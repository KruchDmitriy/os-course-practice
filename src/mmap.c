#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

#include <stdint.h>

#include "mmap.h"
#include "io.h"

extern uint32_t mboot_info;
extern char text_phys_begin[];
extern char bss_phys_end[];

mmap_t memory_map[8];

void init_memory_map() {
	multiboot_info_t* mbi = (multiboot_info_t*)(uint64_t)mboot_info;

	if (! CHECK_FLAG (mbi->flags, 6)) {
		printf("mmap is not valid\n");
		return;
	}

	multiboot_memory_map_t* mmap_addr =
		(multiboot_memory_map_t *)(uint64_t)mbi->mmap_addr;
	multiboot_uint32_t length = mbi->mmap_length;

	printf("mmap_addr = %x, mmap_length = %u\n", mmap_addr, length);

	uint64_t kernel_begin = (uint64_t)text_phys_begin;
	uint64_t kernel_end   = (uint64_t)bss_phys_end;

	printf("text_phys_begin = %llx\n", kernel_begin);
	printf("bss_phys_end    = %llx\n", kernel_end);

	int mmap_idx = 0;

	multiboot_memory_map_t* mmap;
	for (mmap = mmap_addr;
	     (uint64_t)mmap < ((uint64_t)mmap_addr + length);
	     mmap = (multiboot_memory_map_t *) (
	    	(uint64_t)mmap + mmap->size + sizeof(mmap->size))){

		if (mmap->type == MULTIBOOT_MEMORY_RESERVED) {
			continue;
		}

		if (((uint64_t)mmap->addr + mmap->len) < kernel_begin ||
			(uint64_t)mmap->addr > kernel_end) {
			(memory_map[mmap_idx]).addr = mmap->addr;
			(memory_map[mmap_idx]).len  = mmap->len;
			mmap_idx++;

			continue;
		}

		if ((mmap->addr < kernel_begin) &&
			((mmap->addr + mmap->len) > kernel_end)) {
			(memory_map[mmap_idx]).addr = mmap->addr;
			(memory_map[mmap_idx]).len  = kernel_begin - mmap->addr;
			mmap_idx++;

			(memory_map[mmap_idx]).addr = kernel_end;
			(memory_map[mmap_idx]).len  = (mmap->addr + mmap->len) - kernel_end;
			mmap_idx++;
		}
	}


	for (int i = 0; i < mmap_idx; i++) {
		printf("address: %llx, length: %llx\n", memory_map[i]);
	}
}
