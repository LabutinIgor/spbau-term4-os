#include "pit.h"

void init_pit() {
	out8(0x43, 0x34);  //initialize Control Port with 0b00110100
	out8(0x40, 0xff00 & 0xff);
	out8(0x40, (0xff00 >> 8) & 0xff);
}

void pit_handler() {
    puts_uart("OK\n");
    send_eoi(0);
}
