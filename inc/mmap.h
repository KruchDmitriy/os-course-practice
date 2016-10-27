#ifndef __MMAP_H__
#define __MMAP_H__

void init_memory_map();

struct mmap {
	uint64_t addr;
	uint64_t len;
};
typedef struct mmap mmap_t;

typedef unsigned short      multiboot_uint16_t;
typedef unsigned int        multiboot_uint32_t;
typedef unsigned long long  multiboot_uint64_t;

struct multiboot_header {
	multiboot_uint32_t magic;

	multiboot_uint32_t flags;

	multiboot_uint32_t checksum;

	multiboot_uint32_t header_addr;
	multiboot_uint32_t load_addr;
	multiboot_uint32_t load_end_addr;
	multiboot_uint32_t bss_end_addr;
	multiboot_uint32_t entry_addr;

	multiboot_uint32_t mode_type;
	multiboot_uint32_t width;
	multiboot_uint32_t height;
	multiboot_uint32_t depth;
};

struct multiboot_aout_symbol_table {
       multiboot_uint32_t tabsize;
       multiboot_uint32_t strsize;
       multiboot_uint32_t addr;
       multiboot_uint32_t reserved;
};
typedef struct multiboot_aout_symbol_table multiboot_aout_symbol_table_t;

struct multiboot_elf_section_header_table {
    multiboot_uint32_t num;
    multiboot_uint32_t size;
    multiboot_uint32_t addr;
    multiboot_uint32_t shndx;
};
typedef struct multiboot_elf_section_header_table multiboot_elf_section_header_table_t;

struct multiboot_info {
	/* Multiboot info version number */
	multiboot_uint32_t flags;

	/* Available memory from BIOS */
	multiboot_uint32_t mem_lower;
	multiboot_uint32_t mem_upper;

	/* "root" partition */
	multiboot_uint32_t boot_device;

	/* Kernel command line */
	multiboot_uint32_t cmdline;

	/* Boot-Module list */
	multiboot_uint32_t mods_count;
	multiboot_uint32_t mods_addr;

	union
	{
	 multiboot_aout_symbol_table_t aout_sym;
	 multiboot_elf_section_header_table_t elf_sec;
	} u;

	/* Memory Mapping buffer */
	multiboot_uint32_t mmap_length;
	multiboot_uint32_t mmap_addr;

	/* Drive Info buffer */
	multiboot_uint32_t drives_length;
	multiboot_uint32_t drives_addr;

	/* ROM configuration table */
	multiboot_uint32_t config_table;

	/* Boot Loader Name */
	multiboot_uint32_t boot_loader_name;

	/* APM table */
	multiboot_uint32_t apm_table;

	/* Video */
	multiboot_uint32_t vbe_control_info;
	multiboot_uint32_t vbe_mode_info;
	multiboot_uint16_t vbe_mode;
	multiboot_uint16_t vbe_interface_seg;
	multiboot_uint16_t vbe_interface_off;
	multiboot_uint16_t vbe_interface_len;
};
typedef struct multiboot_info multiboot_info_t;

struct multiboot_mmap_entry {
    multiboot_uint32_t size;
    multiboot_uint64_t addr;
    multiboot_uint64_t len;
#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED  2
    multiboot_uint32_t type;
} __attribute__((packed));
typedef struct multiboot_mmap_entry multiboot_memory_map_t;

#endif /*__MMAP_H__*/
