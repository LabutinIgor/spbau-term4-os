#include "interrupt.h"

typedef void (*t_handler_asm)(void);

struct interrupt_descriptor idt[256];
struct idt_ptr idtp;
void handler();

void set_descriptor(int id, t_handler_asm handler_asm) {
    uint64_t offset = (uint64_t) handler_asm;
    idt[id].offset_15_0 = (offset & 0xffff);
    idt[id].segment_selector = 0x18;
    idt[id].ist = 0;
    idt[id].flags = 0x8e;  //0b1110 | 0b10000000
    idt[id].offset_31_16 = ((offset & 0xffff0000) >> 16);
    idt[id].offset_63_32 = (offset >> 32);
    idt[id].reserved = 0;
}

void init_idt() {
    idtp.base = (uint64_t) idt;
    idtp.size = sizeof(idt) - 1;
    set_idt(&idtp);
    set_descriptor(32, handler);
}
