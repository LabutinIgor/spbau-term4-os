#include "pic.h"

void init_pic() {
    //initialize master
    out8(0x20, 0x11);  //0b00010001
    out8(0x21, 0x20);
    out8(0x21, 0x04);  //0b00000100
    out8(0x21, 0x01);  //0b00000001

    //initialize slave
    out8(0xA0, 0x11);  //0b00010001
    out8(0xA1, 0x28);
    out8(0xA1, 0x02);  //0b00000010
    out8(0xA1, 0x01);  //0b00000001
}

void send_eoi() {
    out8(0x20, 0x20);  //send EOI to master
}
