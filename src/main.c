static void qemu_gdb_hang(void)
{
#ifdef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}

#include <desc.h>
#include "ioport.h"
#include "ints.h"
#include "io.h"
#include "pic.h"
#include "pit.h"

void main(void)
{
	qemu_gdb_hang();

	struct desc_table_ptr ptr = { 0 , 0 };
    init_idtr(&ptr);
	write_idtr(&ptr);

    init_serial();
    init_pic();
    disguise_master(0xFE);
    disguise_slave(0xFF);

    enable_ints();

    init_pit();

	while (1);
}
