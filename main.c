#include "uart.h"
#include "pic.h"

void main(void) { 
    init_uart();
    puts_uart("uart initialized\n");
    init_pic();
    puts_uart("pic initialized\n");



    while (1);
}
