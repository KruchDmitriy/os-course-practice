static void qemu_gdb_hang(void)
{
#ifdef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}

#include <desc.h>
#include "ioport.h"
#include "io.h"

void main(void)
{
	qemu_gdb_hang();

	struct desc_table_ptr ptr = { 0 , 0 };

    init_idtr(&ptr);

	write_idtr(&ptr);

    init_serial();

    __asm__(" int $1 ");
	while (1);
}
