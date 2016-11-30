#ifndef __INTS_H__
#define __INTS_H__

#define PIC_MASTER			0x20		/* IO base address for master PIC */
#define PIC_SLAVE			0xA0		/* IO base address for slave PIC */
#define PIC_MASTER_COMMAND	PIC_MASTER
#define PIC_MASTER_DATA		(PIC_MASTER+1)
#define PIC_SLAVE_COMMAND	PIC_SLAVE
#define PIC_SLAVE_DATA		(PIC_SLAVE+1)

#define IDT_SIZE 33

#include <stdint.h>
#include "desc.h"
#include "ioport.h"

typedef struct{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rsp;
    uint64_t rbp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t int_no;
    uint64_t int_error;
} __attribute__((packed)) stack_frame;

typedef struct {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_2;
    uint32_t offset_3;
    uint32_t zero;
} __attribute__((packed)) idt_descr;

static inline void init_pic() {
	out8(PIC_MASTER_COMMAND, 17);
	out8(PIC_SLAVE_COMMAND,  17);

	out8(PIC_MASTER_DATA, 32);
	out8(PIC_SLAVE_DATA,  40);

	out8(PIC_MASTER_DATA, 4);
	out8(PIC_SLAVE_DATA,  2);

	out8(PIC_MASTER_DATA, 1);
	out8(PIC_SLAVE_DATA,  1);
}

static inline void mask_master(uint8_t mask) {
	out8(PIC_MASTER_DATA, mask);
}

static inline void mask_slave(uint8_t mask) {
	out8(PIC_SLAVE_DATA, mask);
}

static inline void disable_ints(void)
{ __asm__ volatile ("cli" : : : "cc"); }

static inline void enable_ints(void)
{ __asm__ volatile ("sti" : : : "cc"); }

idt_descr create_idtr_record(uint64_t handler, int int_gate);

void c_handler(stack_frame* frame);

void init_idtr(struct desc_table_ptr* ptr);

void ints_setup();

#endif /*__INTS_H__*/
