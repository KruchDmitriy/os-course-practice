#include <ioport.h>
#include <memory.h>
#include <balloc.h>
#include <paging.h>
#include <debug.h>
#include <alloc.h>
#include <io.h>
#include <ints.h>
#include <pit.h>
#include <spinlock.h>
#include <threads.h>

// #define DEBUG

static void qemu_gdb_hang(void)
{
#ifdef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}

static void test_kmap(void)
{
	const size_t count = 1024;
	struct page **pages = mem_alloc(sizeof(*pages) * count);
	size_t i;

	BUG_ON(!pages);
	for (i = 0; i != count; ++i) {
		pages[i] = __page_alloc(0);
		if (!pages[i])
			break;
	}

	char *ptr = kmap(pages, i);

	BUG_ON(!ptr);
	BUG_ON((uintptr_t)ptr < HIGHER_BASE);

	for (size_t j = 0; j != i * PAGE_SIZE; ++j)
		ptr[i] = 13;

	for (size_t j = 0; j != i * PAGE_SIZE; ++j)
		BUG_ON(ptr[i] != 13);

	kunmap(ptr);
	mem_free(pages);
}

static void test_alloc(void)
{
	struct list_head head;
	unsigned long count = 0;

	list_init(&head);
	while (1) {
		struct list_head *node = mem_alloc(sizeof(*node));

		if (!node)
			break;
		BUG_ON((uintptr_t)node < HIGHER_BASE);
		++count;
		list_add(node, &head);
	}

	printf("Allocated %lu bytes\n", count * sizeof(head));

	while (!list_empty(&head)) {
		struct list_head *node = head.next;

		BUG_ON((uintptr_t)node < HIGHER_BASE);
		list_del(node);
		mem_free(node);
	}

	mem_alloc_shrink();
}

static void test_slab(void)
{
	struct list_head head;
	struct mem_cache cache;
	unsigned long count = 0;

	list_init(&head);
	mem_cache_setup(&cache, sizeof(head), sizeof(head));
	while (1) {
		struct list_head *node = mem_cache_alloc(&cache);

		if (!node)
			break;
		BUG_ON((uintptr_t)node < HIGHER_BASE);
		++count;
		list_add(node, &head);
	}

	printf("Allocated %lu bytes\n", count * sizeof(head));

	while (!list_empty(&head)) {
		struct list_head *node = head.next;

		BUG_ON((uintptr_t)node < HIGHER_BASE);
		list_del(node);
		mem_cache_free(&cache, node);
	}

	mem_cache_release(&cache);
}

static void test_buddy(void)
{
	struct list_head head;
	unsigned long count = 0;

	list_init(&head);
	while (1) {
		struct page *page = __page_alloc(0);

		if (!page)
			break;
		++count;
		list_add(&page->ll, &head);
	}

	printf("Allocated %lu pages\n", count);

	while (!list_empty(&head)) {
		struct list_head *node = head.next;
		struct page *page = CONTAINER_OF(node, struct page, ll);

		list_del(&page->ll);
		__page_free(page, 0);
	}
}

void test_thread_func(void *argv) {
    printf("This is thread with val %d\n", *((int *)argv));
}

void test_threads() {
	int *vars = mem_alloc(254);
    for (int i = 0; i < 254; i++) {
    	vars[i] = i;
    	create_thread(test_thread_func, &vars[i]);
    }
    vars[254] = 254;
    pid_t first_thread = create_thread(test_thread_func, &vars[254]);

    printf("Threads started\n");
    run_thread(first_thread);
    printf("Threads ended\n");
    mem_free(vars);
}

void main(void *bootstrap_info)
{
    qemu_gdb_hang();

    serial_setup();
    ints_setup();
    pit_setup();
    balloc_setup(bootstrap_info);
    paging_setup();
    page_alloc_setup();
    mem_alloc_setup();
    kmap_setup();

    /* TEMP */
    mask_master(0xFF);
    mask_slave(0xFF);

    // enable_ints();

    init_desc_table();

	printf("Tests Begin\n");

	test_threads();
	test_buddy();
	test_slab();
	test_alloc();
	test_kmap();


	printf("Tests Finished\n");

	while (1);
}
