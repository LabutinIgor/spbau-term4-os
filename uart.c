#include "uart.h"

void init_uart() {
    out8(0x3f8 + 3, 3 | (1 << 7));  //initialize Line Control Register to change divisor
    out8(0x3f8 + 0, 0x0001u & 0xff);  //initialize low byte of divisor
    out8(0x3f8 + 1, 0x0001u & 0xff);  //initialize high byte of divisor
    out8(0x3f8 + 3, 3);  //initialize Line Control Register
    out8(0x3f8 + 1, 0);  //initialize Interrupt Enable Register
}

void putc_uart(char c) {
    while (!((in8(0x3f8 + 5) >> 5) & 1)) {}
    out8(0x3f8 + 0, c);
}

void puts_uart(const char* s) {
    int i = 0;
    while (s[i] != 0) {
        putc_uart(s[i++]);
    }
}
