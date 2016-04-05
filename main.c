#include "kmem_cache.h"
#include "interrupt.h"
#include "memory.h"
#include "serial.h"
#include "paging.h"
#include "stdio.h"
#include "misc.h"
#include "time.h"
#include "threads.h"

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
#define PAGES 10
	struct page *page[PAGES];
	int order[PAGES];

	for (int i = 0; i != PAGES; ++i) {
		page[i] = alloc_pages(i);
		if (page[i]) {
			const phys_t begin = page_paddr(page[i]);
			const phys_t end = begin + (PAGE_SIZE << i);
			printf("allocated [%#llx-%#llx]\n", begin, end - 1);
			order[i] = i;
		}
	}

	for (int i = 0; i != PAGES - 1; ++i) {
		if (!page[i])
			break;
		for (int j = i + 1; j != PAGES; ++j) {
			if (!page[j])
				break;

			const phys_t ibegin = page_paddr(page[i]);
			const phys_t iend = ibegin + (PAGE_SIZE << order[i]);

			const phys_t jbegin = page_paddr(page[j]);
			const phys_t jend = jbegin + (PAGE_SIZE << order[j]);

			DBG_ASSERT(!range_intersect(ibegin, iend, jbegin, jend));
		}
	}

	for (int i = 0; i != PAGES; ++i) {
		if (!page[i])
			continue;

		const phys_t begin = page_paddr(page[i]);
		const phys_t end = begin + (PAGE_SIZE << i);
		printf("freed [%#llx-%#llx]\n", begin, end - 1);
		free_pages(page[i], order[i]);
	}
#undef PAGES
}

struct intlist {
	struct list_head link;
	int data;
};

static void slab_smoke_test(void)
{
#define ALLOCS 1000000
	struct kmem_cache *cache = KMEM_CACHE(struct intlist);
	LIST_HEAD(head);
	int i;

	for (i = 0; i != ALLOCS; ++i) {
		struct intlist *node = kmem_cache_alloc(cache);

		if (!node)
			break;
		node->data = i;
		list_add_tail(&node->link, &head);
	}

	printf("Allocated %d nodes\n", i);

	while (!list_empty(&head)) {
		struct intlist *node = LIST_ENTRY(list_first(&head),
					struct intlist, link);

		list_del(&node->link);
		kmem_cache_free(cache, node);
	}

	kmem_cache_destroy(cache);
#undef ALLOCS
}

void foo1(void* arg) {
	for (int i = 0; i < (long long) arg; i++) {
		printf("foo1: %d\n", i);
		local_irq_disable();
		if (i % 5 == 0)
			scedule();
		local_irq_enable();
	}
}

void foo2(void* arg) {
	for (int i = 0; i < (long long) arg; i++) {
		printf("foo2: %d\n", i);
		local_irq_disable();
		if (i % 5 == 0)
			scedule();
		local_irq_enable();
	}
}

int descriptor1;

void foo1_lock(void* arg) {
	lock(&descriptor1);
	for (int i = 0; i < (long long) arg; i++) {
		printf("foo1_lock: %d\n", i);
		local_irq_disable();
		scedule();
		local_irq_enable();
	}
	unlock(&descriptor1);
}

void foo2_lock(void* arg) {
	lock(&descriptor1);
	for (int i = 0; i < (long long) arg; i++) {
		printf("foo2_lock: %d\n", i);
		local_irq_disable();
		scedule();
		local_irq_enable();
	}
	unlock(&descriptor1);
}

void foo2_join(void* arg) {
	for (int i = 0; i < (long long) arg; i++) {
		printf("foo2_join: %d\n", i);
	}
}

void foo1_join(void* arg) {
	printf("create thread foo2_join\n");
	pid_t foo2_join_pid = create_thread(foo2_join, (void*) 5);
	join_thread(foo2_join_pid);
	printf("joined thread foo2_join\n");
	for (int i = 0; i < (long long) arg; i++) {
		printf("foo1_join: %d\n", i);
	}
}

void threads_test() {
	printf("create thread foo1\n");
	pid_t foo1_pid = create_thread(foo1, (void*) 10);
	printf("create thread foo2\n");
	pid_t foo2_pid = create_thread(foo2, (void*) 5);
	printf("threads foo1 and foo2 created\n");
	
	join_thread(foo1_pid);
	printf("joined thread foo1\n");
	join_thread(foo2_pid);
	printf("joined thread foo2\n");


	printf("create thread foo1_lock\n");
	pid_t foo1_lock_pid = create_thread(foo1_lock, (void*) 5);
	printf("create thread foo2_lock\n");
	pid_t foo2_lock_pid = create_thread(foo2_lock, (void*) 5);
	printf("threads foo1_lock and foo2_lock created\n");
	
	join_thread(foo1_lock_pid);
	printf("joined thread foo1_lock\n");
	join_thread(foo2_lock_pid);
	printf("joined thread foo2_lock\n");

	printf("create thread foo1_join\n");
	pid_t foo1_join_pid = create_thread(foo1_join, (void*) 5);
	join_thread(foo1_join_pid);
	printf("joined thread foo1_join\n");
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

	setup_threads();
	setup_time();

	buddy_smoke_test();
	slab_smoke_test();

	local_irq_enable();
	threads_test();



	while (1);
}
