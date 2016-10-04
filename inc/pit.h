#ifndef __PIT_H__
#define __PIT_H__

#define PIT_COMMAND 0x43
#define PIT_DATA 	0x40

#include "ioport.h"

static inline void init_pit() {
	out8(PIT_COMMAND, 48);
	out8(PIT_DATA, 0xFF);
	out8(PIT_DATA, 0xFF);
}

#endif /*__PIT_H__*/