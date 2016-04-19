#include "kmem_cache.h"
#include "interrupt.h"
#include "threads.h"
#include "memory.h"
#include "serial.h"
#include "paging.h"
#include "stdio.h"
#include "misc.h"
#include "time.h"
#include "file_system.h"

static bool range_intersect(phys_t l0, phys_t r0, phys_t l1, phys_t r1)
{
    if (r0 <= l1)
        return false;
    if (r1 <= l0)
        return false;
    return true;
}

static void buddy_smoke_test(void)
{
    DBG_INFO("Start buddy test");
    struct page *page[10];
    int order[ARRAY_SIZE(page)];

    for (int i = 0; i != ARRAY_SIZE(page); ++i) {
        page[i] = alloc_pages(i);
        if (page[i]) {
            const phys_t begin = page_paddr(page[i]);
            const phys_t end = begin + (PAGE_SIZE << i);
            DBG_INFO("allocated [%#llx-%#llx]", begin, end - 1);
            order[i] = i;
        }
    }

    for (int i = 0; i != ARRAY_SIZE(page) - 1; ++i) {
        if (!page[i])
            break;
        for (int j = i + 1; j != ARRAY_SIZE(page); ++j) {
            if (!page[j])
                break;

            const phys_t ibegin = page_paddr(page[i]);
            const phys_t iend = ibegin + (PAGE_SIZE << order[i]);

            const phys_t jbegin = page_paddr(page[j]);
            const phys_t jend = jbegin + (PAGE_SIZE << order[j]);

            DBG_ASSERT(!range_intersect(ibegin, iend, jbegin, jend));
        }
    }

    for (int i = 0; i != ARRAY_SIZE(page); ++i) {
        if (!page[i])
            continue;

        const phys_t begin = page_paddr(page[i]);
        const phys_t end = begin + (PAGE_SIZE << i);
        DBG_INFO("freed [%#llx-%#llx]", begin, end - 1);
        free_pages(page[i], order[i]);
    }
    DBG_INFO("Buddy test finished");
}

struct intlist {
    struct list_head link;
    int data;
};

static void slab_smoke_test(void)
{
    DBG_INFO("Start SLAB test");
    struct kmem_cache *cache = KMEM_CACHE(struct intlist);
    LIST_HEAD(head);
    int i;

    for (i = 0; i != 1000000; ++i) {
        struct intlist *node = kmem_cache_alloc(cache);

        if (!node)
            break;
        node->data = i;
        list_add_tail(&node->link, &head);
    }

    DBG_INFO("Allocated %d nodes", i);

    while (!list_empty(&head)) {
        struct intlist *node = LIST_ENTRY(list_first(&head),
                    struct intlist, link);

        list_del(&node->link);
        kmem_cache_free(cache, node);
    }

    kmem_cache_destroy(cache);
    DBG_INFO("SLAB test finished");
}

static int test_function(void *dummy)
{
    (void) dummy;
    return 0;
}

static void test_threading(void)
{
    DBG_INFO("Start threading test");
    for (int i = 0; i != 10000; ++i) {
        const pid_t pid = create_kthread(&test_function, 0);

        DBG_ASSERT(pid >= 0);
        wait(pid);
    }
    DBG_INFO("Threading test finished");
}

static void copy_str(char* str1, const char* str2) {
    int i = 0;
    while (str2[i] != 0) {
        str1[i] = str2[i];
        i++;
    }
    str1[i] = 0;
}

static void test_file_system() {
    DBG_INFO("Start file_system test");
    
    printf("Open file a.txt\n");
    struct file* file1 = open("", "a.txt");
    
    char buffer[8];
    char read_buffer[4];
    for (int i = 0; i < 4; i++) {
        read_buffer[i] = 0;
    }
    copy_str(buffer, "abacaba");

    printf("Write %s to a.txt\n", buffer);
    write(file1, buffer, 0, 7);

    printf("Read from a.txt\n");
    read(file1, read_buffer, 1, 3);
    printf("Read 1..3 chars: %s\n", read_buffer);

    printf("Make some directories\n");
    mkdir("", "dir1");
    mkdir("dir1", "dir2");
    mkdir("dir1/dir2", "dir3");
    mkdir("dir1/dir2", "dir4");

    printf("readdir dir1/dir2:\n");
    struct fs_node* node = readdir("dir1/dir2");
    while (node != 0) {
        printf("%s\n", node->name);
        node = node->next_node;
    }


    DBG_INFO("file_system test finished");  
}

static int start_kernel(void *dummy)
{
    (void) dummy;

    buddy_smoke_test();
    slab_smoke_test();
    test_threading();
    test_file_system();

    return 0;
}

void main(void)
{
    setup_serial();
    setup_misc();
    setup_ints();
    setup_memory();
    setup_buddy();
    setup_paging();
    setup_alloc();
    setup_time();
    setup_threading();
    local_irq_enable();

    create_kthread(&start_kernel, 0);
    local_preempt_enable();

    setup_fs();

    idle();

    while (1);
}
