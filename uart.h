#pragma once

#include "ioport.h"

void init_uart();
void putc_uart(char c);
void puts_uart(const char* s);
