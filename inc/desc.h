#ifndef __DESC_H__
#define __DESC_H__
#define IDT_SIZE 33

#include <stdint.h>

#include "memory.h"

struct desc_table_ptr {
	uint16_t size;
	uint64_t addr;
} __attribute__((packed));

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

static inline void read_idtr(struct desc_table_ptr *ptr)
{
	__asm__ ("sidt %0" : "=m"(*ptr));
}

static inline void write_idtr(const struct desc_table_ptr *ptr)
{
	__asm__ ("lidt %0" : : "m"(*ptr));
}

static inline void read_gdtr(struct desc_table_ptr *ptr)
{
	__asm__ ("sgdt %0" : "=m"(*ptr));
}

static inline void write_gdtr(const struct desc_table_ptr *ptr)
{
	__asm__ ("lgdt %0" : : "m"(*ptr));
}

idt_descr create_idtr_record(uint64_t handler, int int_gate);

void c_handler(stack_frame* frame);

void init_idtr(struct desc_table_ptr* ptr);

#endif /*__DESC_H__*/
