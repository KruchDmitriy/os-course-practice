#include <string.h>
#include <alloc.h>
#include <io.h>
#include <fs.h>

struct file_desc {
    struct node* file_node;
    size_t cur_pos;
    int    is_empty;
};

typedef struct file_desc file_desc_table[MAX_FILE_DESCS];

static struct node *root;
static file_desc_table fd_table;
static struct spinlock locking_var;


static int parse_path(const char *path, char **names[]) {
    size_t count_slashes = 0;
    size_t size = strlen(path);

    if (size == 0) { return 0; }

    size_t i;
    for (i = 0; i <= size; i++) {
        if (path[i] == '/') { count_slashes++; }
    }

    char **names_ = (char **) mem_alloc((count_slashes + 1) * sizeof(char *));

    size_t name_len = 0;
    size_t name_idx = 0;
    for (i = 0; i <= size && name_idx <= count_slashes; i++) {
        if (path[i] != '/' && i != size) {
            name_len++;
            continue;
        }

        names_[name_idx] = (char *) mem_alloc(name_len + 1);
        memcpy(names_[name_idx], path + i - name_len, name_len);
        names_[name_idx][name_len] = '\0';

        name_idx++;
        name_len = 0;
    }

    *names = names_;

    if (path[size - 1] == '/') {
        return (int) count_slashes;
    } else {
        return (int) count_slashes + 1;
    }
}

static void print_node(struct node *cur_node, int shift) {
    for (int i = 0; i < shift; ++i) {
        printf("    ");
    }

    if (cur_node->type == DIR) {
        printf("%s/\n", cur_node->name);

        struct node *node_in_dir = cur_node->child;
        while (node_in_dir != NULL) {
            print_node(node_in_dir, shift + 1);
            node_in_dir = node_in_dir->next;
        }
    } else {
        printf("%s\n", cur_node->name);
    }
}

void print_file_system() {
    print_node(root, 0);
}


void init_file_system() {
    root = (struct node*)mem_alloc(sizeof(struct node));

    root->name = "";
    root->type = DIR;

    root->next = NULL;
    root->child = NULL;
    root->addr = NULL;
    root->size = 0;

    for (int i = 0; i < MAX_FILE_DESCS; i++) {
        fd_table[i].is_empty = 1;
    }
}

static struct node *create_new_node(struct node *prev, int is_parent,
                                    const char *name, enum node_type type)
{
    struct node* new_node = (struct node*)mem_alloc(sizeof(struct node));

    if (is_parent) {
        if (prev->child != NULL) {
            struct node *tmp = prev->child;
            prev->child = new_node;
            new_node->next = tmp;
        } else {
            prev->child = new_node;
            new_node->next = NULL;
        }
    } else {
        if (prev->next != NULL) {
            struct node *tmp = prev->next;
            prev->next = new_node;
            new_node->next = tmp;
        } else {
            prev->next = new_node;
            new_node->next = NULL;
        }
    }

    new_node->name = (char *) mem_alloc(strlen(name) + 1);
    memcpy(new_node->name, name, strlen(name) + 1);
    new_node->child = NULL;
    new_node->addr = NULL;
    new_node->size = 0;
    new_node->type = type;

    return new_node;
}

static struct node *find_node(const char *path, enum node_type type) {
    struct node *cur_node = root;

    char **names_in_path = 0;
    int cnt_names = parse_path(path, &names_in_path);

    int i;
    for (i = 0; i < cnt_names; i++) {
        while (1) {
            if (cur_node == NULL) {
                int j;
                for (j = 0; j < cnt_names; j++) {
                    mem_free(names_in_path[j]);
                }
                mem_free(names_in_path);
                return NULL;
            }

            if (strcmp(cur_node->name, names_in_path[i]) == 0 && cur_node->type == DIR) {
                cur_node = cur_node->child;
                break;
            }

            if (strcmp(cur_node->name, names_in_path[i]) == 0 &&
                    i == cnt_names - 1 && cur_node->type == type) {
                break;
            }

            cur_node = cur_node->next;
        }
    }

    int j;
    for (j = 0; j < cnt_names; j++) {
        mem_free(names_in_path[j]);
    }
    mem_free(names_in_path);
    return cur_node;
}


static struct node *create_node(const char *path, enum node_type type) {
    struct node *cur_node = root;
    struct node *prev_node = root;

    if (find_node(path, type) != NULL) {
        printf("File or directory already exists\n");
        return NULL;
    }

    char **names_in_path = 0;
    int cnt_names = parse_path(path, &names_in_path);

    int i;
    for (i = 0; i < cnt_names - 1; i++) {
        while (1) {
            if (cur_node == NULL) {
                printf("Unknown directories on path to file\n");
                int j;
                for (j = 0; j < cnt_names; j++) {
                    mem_free(names_in_path[j]);
                }
                mem_free(names_in_path);
                return NULL;
            }

            if (strcmp(cur_node->name, names_in_path[i]) == 0 &&
                cur_node->type == DIR) {
                prev_node = cur_node;
                cur_node = cur_node->child;
                break;
            }

            prev_node = cur_node;
            cur_node = cur_node->next;
        }
    }


    char* name = names_in_path[i];
    struct node* new_node;
    if (prev_node->type == DIR) {
        new_node = create_new_node(prev_node, 1, name, type);
    } else {
        new_node = create_new_node(prev_node, 0, name, type);
    }

    int j;
    for (j = 0; j < cnt_names; j++) {
        mem_free(names_in_path[j]);
    }
    mem_free(names_in_path);

    return new_node;
}


int open(const char *filename, int flag) {
   lock(&locking_var);

    int fd = -1;
    for (int i = 0; i < MAX_FILE_DESCS; i++) {
        if (fd_table[i].is_empty == 1) {
            fd_table[i].is_empty = 0;
            fd = i;
            break;
        }
    }

    if (fd == -1) {
        printf("Cannot open file, no mem_free file descriptors!\n");
        unlock(&locking_var);
        return -1;
    }

    struct file_desc *desc = &fd_table[fd];
    struct node *cur_node;
    if (flag == F_READ) {
        cur_node = find_node(filename, FILE);
    } else {
        cur_node = create_node(filename, FILE);
    }

    if (cur_node == NULL) {
        desc->is_empty = 1;
        unlock(&locking_var);
        return -1;
    } else {
        desc->file_node = cur_node;
        desc->cur_pos = 0;

        unlock(&locking_var);
        return fd;
    }
}

void close(int fd) {
    lock(&locking_var);
    fd_table[fd].file_node = NULL;
    fd_table[fd].cur_pos = 0;
    fd_table[fd].is_empty = 1;
    unlock(&locking_var);
}

int read(int fd, void *buf, int count) {
    lock(&locking_var);
    struct node *file_node = fd_table[fd].file_node;
    unlock(&locking_var);

    lock(&file_node->locking_var);

    char *addr = (char *) file_node->addr;
    int   size = (int) file_node->size;
    int   start_pos = (int) fd_table[fd].cur_pos;
    char *dst_addr  = (char *) buf;

    int i = start_pos;
    while (i <= count && i < size) {
        dst_addr[i] = addr[i];
        i++;
    }

    fd_table[fd].cur_pos = (size_t)i;

    unlock(&file_node->locking_var);

    return i - start_pos;
}

int write(int fd, const void *buf, int count) {
    lock(&locking_var);
    struct node *cur_node = fd_table[fd].file_node;
    unlock(&locking_var);

    lock(&cur_node->locking_var);

    size_t cur_pos = fd_table[fd].cur_pos;
    size_t size = cur_node->size;

    if (cur_pos + count >= size) {
        if (cur_pos + count < size * 2) {
            size = (size + 1) * 2;
            cur_node->addr = mem_realloc(cur_node->addr, size);

            cur_node->size = size;
        } else {
            size = cur_pos + count + 1;
            cur_node->addr = mem_realloc(cur_node->addr, size);
            cur_node->size = size;
        }
    }

    char *addr = (char *) cur_node->addr;
    char *dst_addr = (char *) buf;
    int i = 0;
    while (i < count) {
        addr[i + cur_pos] = dst_addr[i];
        i++;
    }

    unlock(&cur_node->locking_var);

    return count;
}

int mkdir(const char *path) {
    lock(&locking_var);
    if (find_node(path, DIR) != NULL) {
        printf("Directory already exists");
        unlock(&locking_var);
        return 0;
    }

    struct node *new_dir = create_node(path, DIR);
    unlock(&locking_var);
    return new_dir != NULL;
}

struct node *opendir(const char *path) {
    lock(&locking_var);
    struct node *opened_dir = find_node(path, DIR);
    if (opened_dir == NULL) {
        unlock(&locking_var);
        return NULL;
    } else {
        unlock(&locking_var);
        return opened_dir->child;
    }
}

struct node *readdir(struct node *cur_dir) {
    lock(&locking_var);
    if (cur_dir == NULL) { return NULL; }
    struct node *next = cur_dir->next;
    unlock(&locking_var);
    return next;
}
