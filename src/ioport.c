#include "ioport.h"

void init_serial() {
// turn off interruptions

// write to 0x3f8 + 1
    out8(0x3f9, 0);

// set div coeff
// write to 0x3f8 + 3, 7th bit
    out8(0x3fb, 128);
// write to +0, then +1 2 bytes
    out8(0x3f8, 0xe1);
    out8(0x3f9, 0);

// set frame format
// write to +3, 3
    out8(0x3fb, 3);
}

void print(char a) {
    out8(0x3f8, a);
}
