#include "memory_map.h"

extern const uint32_t mboot_info;

extern char text_phys_begin[];
extern char bss_phys_end[];

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

multiboot_info_t* mbi;
multiboot_memory_map_t entries[33];
int map_size = 0;


int validate_map() {
    mbi = (multiboot_info_t *) (uint64_t) mboot_info;
    return CHECK_FLAG(mbi->flags, 6);
}

void get_map() {
    multiboot_memory_map_t *mmap;

    for (mmap = (multiboot_memory_map_t *) ((uint64_t) mbi->mmap_addr);
        (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
        mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
                                 + mmap->size + sizeof (mmap->size))) {
        entries[map_size++] = *mmap;
    }

    multiboot_memory_map_t os_entry;
    os_entry.size = 20;
    os_entry.addr = (uint64_t) text_phys_begin;
    os_entry.len  = ((uint64_t) bss_phys_end) - ((uint64_t) text_phys_begin) + 1;
    os_entry.type = 2;

    entries[map_size++] = os_entry;

    for (int i = 0; i < map_size; i++) {
        if (entries[i].type == 1) {
            if (entries[i].addr >= os_entry.addr && entries[i].addr + entries[i].len <= os_entry.addr + os_entry.len) {
                entries[i] = entries[--map_size];
                i--;
            } else {
                if (entries[i].addr <= os_entry.addr && entries[i].addr + entries[i].len >= os_entry.addr + os_entry.len) {
                    multiboot_memory_map_t new_free_entry = entries[i];
                    new_free_entry.addr = os_entry.addr + os_entry.len;
                    new_free_entry.len = (entries[i].addr + entries[i].len) - (os_entry.addr + os_entry.len);
                    entries[map_size++] = new_free_entry;

                    entries[i].len = os_entry.addr - entries[i].addr;
                }

                if (entries[i].addr <= os_entry.addr && entries[i].addr + entries[i].len >= os_entry.addr &&
                    entries[i].addr + entries[i].len <= os_entry.addr + os_entry.len) {

                    entries[i].len = os_entry.addr - entries[i].addr;
                }

                if (entries[i].addr >= os_entry.addr && entries[i].addr <= os_entry.addr + os_entry.len &&
                    entries[i].addr + entries[i].len > os_entry.addr + os_entry.len) {

                    entries[i].addr = os_entry.addr + os_entry.len;
                }
            }
        }
    }
}

void print_map() {
    for (int i = 0; i < map_size; i++) {
        multiboot_memory_map_t *mmap = &entries[i];
        printf (" size = 0x%x, base_addr = 0x%x%x,"
                     " length = 0x%x%x, type = 0x%x\n",
                     (unsigned) mmap->size,
                     mmap->addr >> 32,
                     mmap->addr & 0xffffffff,
                     mmap->len >> 32,
                     mmap->len & 0xffffffff,
                     (unsigned) mmap->type);
    }
}