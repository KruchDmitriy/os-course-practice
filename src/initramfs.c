#include <initramfs.h>
#include <io.h>
#include <string.h>
#include <fs.h>
#include <alloc.h>
#include <memory.h>

#define CHECK_FLAG(flags,bit)   ((flags) & (1ul << (bit)))

uintptr_t __irmfs_start = 0;
uintptr_t __irmfs_end   = 0;

struct mboot_info {
    uint32_t flags;
    uint8_t ignore0[16];
    uint32_t mods_count;
    uint32_t mods_addr;
} __attribute__((packed));


struct multiboot_mod_list {
    /* the memory used goes from bytes 'mod_start' to 'mod_end-1' inclusive */
    uint32_t mod_start;
    uint32_t mod_end;

    /* Module command line */
    uint32_t cmdline;

    /* padding to take it to 16 bytes (must be zero) */
    uint32_t pad;
};

typedef struct multiboot_mod_list multiboot_module_t;

void find_initramfs(const struct mboot_info* mbi) {
    printf("%llx", mbi);
    if (CHECK_FLAG (mbi->flags, 3)) {
        multiboot_module_t *mod;
        size_t i;

        printf ("mods_count = %d, mods_addr = %x\n",
               (int) mbi->mods_count, (int) mbi->mods_addr);
        for (i = 0, mod = (multiboot_module_t *)(uint64_t) mbi->mods_addr;
            i < mbi->mods_count;
            i++, mod++)
        {
            printf (" mod_start = %x, mod_end = %x, cmdline = %s\n",
                    (unsigned) mod->mod_start,
                    (unsigned) mod->mod_end,
                    (char *)(uint64_t) mod->cmdline);

            struct cpio_header* addr = (struct cpio_header*)
                                        (uint64_t)mod->mod_start;

            if (mod->mod_end - mod->mod_start > sizeof(struct cpio_header) &&
                strcmp(addr->magic, "070701")) {
                __irmfs_start = (uintptr_t)mod->mod_start;
                __irmfs_end   = (uintptr_t)mod->mod_end;
            }
        }
    }
}

static void align_up(uint8_t** ptr) {
    uint64_t ptr_ = (uint64_t)*ptr;
    if (ptr_ % 4 != 0) {
        ptr_ = ptr_ + (4 - ptr_ % 4);
    }
    *ptr = (uint8_t *)ptr_;
}

static uint32_t atoi(char* s, size_t len) {
    // printf("%s\n",s);
    uint32_t x = 0;
    for (size_t i = 0; i < len; ++i) {
        // printf("%d\n", x);
        x = x * 16;
        if (s[i] >= '0' && s[i] <= '9') {
            x += s[i] - '0';
        } else
          if (s[i] >= 'a' && s[i] <= 'z'){
              x += s[i] - 'a' + 10;
          } else if (s[i] >= 'A' && s[i] <= 'Z') {
              x += s[i] - 'A' + 10;
        }
    }

    return x;
}

void initramfs() {
    uint8_t* start = (uint8_t*)va(__irmfs_start);
    start = start;
    uint8_t* end   = (uint8_t*)va(__irmfs_end);
    uint8_t* ptr = start;
    while (1) {
        align_up(&ptr);
        if (strcmp((char*)ptr, END_OF_ARCHIVE) == 0) {
            printf("end\n");
            return;
        }

        struct cpio_header* header = (struct cpio_header*)ptr;

        size_t name_size = atoi(header->namesize, 8);
        size_t file_size = atoi(header->filesize, 8);

        char* name = (char *)mem_alloc(name_size + 2);
        memcpy(name + 1, ptr + sizeof(struct cpio_header), name_size);
        name[name_size] = '\0';
        name[0] = '/';


        if (strcmp(name + 1, END_OF_ARCHIVE) == 0) {
            printf("end\n");
            return;
        }

        uint8_t* data = ptr + sizeof(struct cpio_header) + name_size;
        align_up(&data);

        if (data + file_size > end) {
            printf("stop\n");
            return;
        }

        if (S_ISREG(atoi(header->mode, 8))) {
            int fd = open(name, F_WRITE);
            write(fd, data, file_size);
            close(fd);
        } else if (S_ISDIR(atoi(header->mode, 8))) {
            mkdir(name);
        }

        mem_free(name);

        ptr = data + file_size;
    }
}
