#include "file_system.h"

struct fs_node root;

void setup_fs() {
    root.is_dir = 1;
}

struct fs_node *go_to_child(struct fs_node* node, const char *child_name) {
    while (node != 0 && node->name != child_name) {
        node = node->next_node;
    }
    return node;
}

void append(char* str, char c) {
    int i = 0;
    while (str[i] != 0) i++;
    str[i] = c;
    str[i + 1] = 0;
}

void copy_str(char* str1, const char* str2) {
    int i = 0;
    while (str2[i] != 0) {
        str1[i] = str2[i];
        i++;
    }
}

void copy_mem(char* mem1, char* mem2, int len) {
    for (int i = 0; i < len; i++) {
        mem1[i] = mem2[i];
    }
}

struct fs_node* find_node(const char* path) {
    if (path[0] == 0) {
        return &root;
    }
    int i = 0;
    struct fs_node* cur_node = &root;
    char cur_name[100];

    while (path[i] != 0) {
        if (path[i] == '/') {
            cur_node = go_to_child(cur_node, cur_name);
            cur_node = cur_node->child;
            cur_name[0] = 0;
        } else {
            append(cur_name, path[i]);
        }
        i++;
    }
    return go_to_child(cur_node, cur_name);
}

struct fs_node* make_file(const char* name) {
    struct fs_node* node = kmem_alloc(sizeof(struct fs_node));

    node->is_dir = 0;
    copy_str(node->name, name);
    node->file = kmem_alloc(sizeof(struct file));

    return node;
}

struct file* open(const char* path, const char* name) {
    //some lock?

    struct fs_node* node = find_node(path);

    if (node->child == 0) {
        node->child = make_file(name);
        return node->child->file;
    }

    node = node->child;
    
    while (node->next_node != 0 && node->name != name) {
        node = node->next_node;
    }

    if (node->name == name) {
        return node->file;
    }
    struct fs_node* new_node = make_file(name);
    node->next_node = new_node;


    return new_node->file;
}

void close(struct file* file) {
    printf("close: %d\n", file->capacity);
    //some unlock?
}

void read(struct file* file, char* buffer, int offset, int len) {
    for (int i = offset; i < offset + len; i++) {
        buffer[i - offset] = file->mem[i];
    }
}

void write(struct file* file, char* buffer, int offset, int len) {
    if (file->capacity < offset + len) {
        if (file->capacity == 0) {
            file->capacity = 100;
        }
        int new_capacity = file->capacity * 2;
        while (new_capacity < offset + len) {
            new_capacity *= 2;
        }

        if (file->mem == 0) {
            file->mem = kmem_alloc(new_capacity);
        } else {
            char* new_mem = kmem_alloc(new_capacity);
            copy_mem(new_mem, file->mem, file->capacity);
            file->mem = new_mem;
        }
        file->capacity = new_capacity;
    }
    
    for (int i = offset; i < offset + len; i++) {
        file->mem[i] = buffer[i - offset];
    }
}

void mkdir(const char* path, const char* name) {
    struct fs_node* node = find_node(path);
    struct fs_node* new_node = kmem_alloc(sizeof(struct fs_node));
    new_node->is_dir = 1;
    copy_str(new_node->name, name);

    new_node->next_node = node->child;
    node->child = new_node;

}

struct fs_node* readdir(const char* path) {
    struct fs_node* node = find_node(path);
    if (!node->is_dir) {
        printf("Error in readdir: no such directory\n");
        return 0;
    }
    return node->child;
}


