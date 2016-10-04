#ifndef __PIC_H__
#define __PIC_H__

#define PIC_MASTER			0x20		/* IO base address for master PIC */
#define PIC_SLAVE			0xA0		/* IO base address for slave PIC */
#define PIC_MASTER_COMMAND	PIC_MASTER
#define PIC_MASTER_DATA		(PIC_MASTER+1)
#define PIC_SLAVE_COMMAND	PIC_SLAVE
#define PIC_SLAVE_DATA		(PIC_SLAVE+1)

#include <stdint.h>
#include "ioport.h"

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

static inline void disguise_master(uint8_t mask) {
	out8(PIC_MASTER_DATA, mask);
}

static inline void disguise_slave(uint8_t mask) {
	out8(PIC_SLAVE_DATA, mask);
}

#endif /*__PIC_H__*/