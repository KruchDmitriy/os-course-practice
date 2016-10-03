#include "desc.h"
#include "tools.h"
#include "ioport.h"

extern uint64_t offsets[];
idt_descr descrs[IDT_SIZE];

idt_descr create_idtr_record(uint64_t handler, int dpl, int int_gate) {
    idt_descr descr;
    descr.selector = KERNEL_CS;
    descr.offset_1 = handler & (0xFFFF);
    descr.offset_2 = (handler & (0xFFFF0000)) >> 16;
    descr.offset_3 = (handler & (0xFFFFFFFF00000000)) >> 32;
    if (int_gate == 1) {
        //descr.type_attr = gen8(
        //    (int[]){1, dpl, dpl % 2, 0,
        //     1, 1, 1, 0 /*0xE* inter_gate */ });
        dpl = dpl;
        descr.type_attr = 0x8E;
    } else {
        //descr.type_attr = gen8(
        //    (int[]){1, dpl, dpl % 2, 0,
        //     1, 1, 1, 1 /*0xF trap_gate*/ });
        descr.type_attr = 0x8F;
    }
    descr.ist = 0;
    descr.zero = 0;
    return descr;
}

void c_handler(stack_frame* frame) {
    print_char(48 + frame->int_no);
}

void init_idtr(struct desc_table_ptr* ptr) {
    for (int i = 0; i < IDT_SIZE; i++) {
        descrs[i] = create_idtr_record(offsets[i], 0, 0);
    }

    ptr->addr = (uint64_t)descrs;
    ptr->size = sizeof(idt_descr)*IDT_SIZE - 1;
}
