#include "desc.h"
#include "io.h"
#include "pic.h"

extern uint64_t offsets[];
idt_descr descrs[IDT_SIZE];

idt_descr create_idtr_record(uint64_t handler, int int_gate) {
    idt_descr descr;
    descr.selector = KERNEL_CS;
    descr.offset_1 = handler & (0xFFFF);
    descr.offset_2 = (handler & (0xFFFF0000)) >> 16;
    descr.offset_3 = (handler & (0xFFFFFFFF00000000)) >> 32;
    if (int_gate == 1) {
        descr.type_attr = 0x8E;
    } else {
        descr.type_attr = 0x8F;
    }
    descr.ist = 0;
    descr.zero = 0;
    return descr;
}

void c_handler(stack_frame* frame) {
    printf("Interruption!");
    switch (frame->int_no) {
        case 0:  printf("%s %d: %s\n", "Number ", 0, "Divide by 0"); break;
        case 1:  printf("%s %d: %s\n", "Number ", 1, "Reserved"); break;
        case 2:  printf("%s %d: %s\n", "Number ", 2, "NMI Interrupt"); break;
        case 3:  printf("%s %d: %s\n", "Number ", 3, "Breakpoint (INT3)"); break;
        case 4:  printf("%s %d: %s\n", "Number ", 4, "Overflow (INTO)"); break;
        case 5:  printf("%s %d: %s\n", "Number ", 5, "Bounds range exceeded (BOUND)"); break;
        case 6:  printf("%s %d: %s\n", "Number ", 6, "Invalid opcode (UD2)"); break;
        case 7:  printf("%s %d: %s\n", "Number ", 7, "Device not available (WAIT/FWAIT)"); break;
        case 8:  printf("%s %d: %s\n", "Number ", 8, "Double fault"); break;
        case 9:  printf("%s %d: %s\n", "Number ", 9, "Coprocessor segment overrun"); break;
        case 10: printf("%s %d: %s\n", "Number ", 10, "Invalid TSS"); break;
        case 11: printf("%s %d: %s\n", "Number ", 11, "Segment not present"); break;
        case 12: printf("%s %d: %s\n", "Number ", 12, "Stack-segment fault"); break;
        case 13: printf("%s %d: %s\n", "Number ", 13, "General protection fault"); break;
        case 14: printf("%s %d: %s\n", "Number ", 14, "Page fault"); break;
        case 15: printf("%s %d: %s\n", "Number ", 15, "Reserved"); break;
        case 16: printf("%s %d: %s\n", "Number ", 16, "x87 FPU error"); break;
        case 17: printf("%s %d: %s\n", "Number ", 17, "Alignment check"); break;
        case 18: printf("%s %d: %s\n", "Number ", 18, "Machine check"); break;
        case 19: printf("%s %d: %s\n", "Number ", 19, "SIMD Floating-Point Exception"); break;
        case 20: printf("%s %d: %s\n", "Number ", 20, "Reserved"); break;
        case 21: printf("%s %d: %s\n", "Number ", 21, "Reserved"); break;
        case 22: printf("%s %d: %s\n", "Number ", 22, "Reserved"); break;
        case 23: printf("%s %d: %s\n", "Number ", 23, "Reserved"); break;
        case 24: printf("%s %d: %s\n", "Number ", 24, "Reserved"); break;
        case 25: printf("%s %d: %s\n", "Number ", 25, "Reserved"); break;
        case 26: printf("%s %d: %s\n", "Number ", 26, "Reserved"); break;
        case 27: printf("%s %d: %s\n", "Number ", 27, "Reserved"); break;
        case 28: printf("%s %d: %s\n", "Number ", 28, "Reserved"); break;
        case 29: printf("%s %d: %s\n", "Number ", 29, "Reserved"); break;
        case 30: printf("%s %d: %s\n", "Number ", 30, "Reserved"); break;
        case 31: printf("%s %d: %s\n", "Number ", 31, "Reserved"); break;
        case 32: {
                printf("%s\n", "Hello from PIT!");
                out8(PIC_MASTER_COMMAND, 32);
                break;
        }
    }
}

void init_idtr(struct desc_table_ptr* ptr) {
    for (int i = 0; i < IDT_SIZE; i++) {
        descrs[i] = create_idtr_record(offsets[i], 0);
    }

    ptr->addr = (uint64_t)descrs;
    ptr->size = sizeof(idt_descr)*IDT_SIZE - 1;
}
