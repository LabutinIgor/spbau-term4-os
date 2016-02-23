#pragma once

#include "ioport.h"
#include "memory.h"

#include <stdint.h>

void init_idt();

struct interrupt_descriptor {
    uint16_t offset_15_0;
    uint16_t segment_selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t offset_31_16;
    uint32_t offset_63_32;
    uint32_t reserved;
} __attribute__((packed));

struct idt_ptr {
    uint16_t size;
    uint64_t base;
} __attribute__((packed));

static inline void set_idt(const struct idt_ptr *ptr)
{ __asm__ volatile ("lidt (%0)" : : "a"(ptr)); }
