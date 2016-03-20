#include "interrupt.h"
#include "serial.h"
#include "stdio.h"
#include "time.h"
#include "memory_map.h"

void main(void)
{
	setup_serial();
	setup_ints();
	setup_time();

	//local_irq_enable();

	if (validate_map()) {
		get_map();
		print_map();
	}

	while (1);
}
