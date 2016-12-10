#ifndef __FS_H__
#define __FS_H__

#include <spinlock.h>

#define MAX_FILE_DESCS 256

#define F_READ   0
#define F_WRITE  1

enum node_type {
    FILE,
    DIR
};

struct node {
    struct node* next;
    struct node* child;

    enum node_type type;
    char*  name;
    void*  addr;
    size_t size;

    struct spinlock locking_var;
};

void init_file_system();
void print_file_system();

int  open (const char* filename, int flag);
void close(int fd);
int  read (int fd, void* buf, int count);
int  write(int fd, const void* buf, int count);

int mkdir(const char* path);
struct node* opendir(const char* path);
struct node* readdir(struct node* dir);

#endif /* __FS_H__ */
