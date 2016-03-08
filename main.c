#include "uart.h"
#include "pic.h"
#include "pit.h"
#include "interrupt.h"

void main(void) { 
    init_uart();
    puts_uart("uart initialized\n");
    init_pic();
    puts_uart("pic initialized\n");
    init_pit();
    puts_uart("pit initialized\n");
    init_idt();
    puts_uart("idt initialized\n");


    

    //__asm__ volatile ("sti");

    //while (1) {
    //    __asm__ volatile ("hlt");
    //}
}
