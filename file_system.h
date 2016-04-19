#pragma once

#include "interrupt.h"
#include "memory.h"
#include "kmem_cache.h"
#include "stdio.h"

struct file {
    char* mem;
    int capacity;
};

struct fs_node {
    struct fs_node *next_node;
    char name[100];
    int is_dir;
    struct fs_node *child;
    struct file *file;
};

void setup_fs();
struct file* open(const char* path, const char* name);
void close(struct file* file);
void read(struct file* file, char* buffer, int offset, int len);
void write(struct file* file, char* buffer, int offset, int len);
void mkdir(const char* path, const char* name);
struct fs_node* readdir(const char* path);
