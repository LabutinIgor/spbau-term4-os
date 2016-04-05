#pragma once

#include "threads.h"
#include "interrupt.h"

void lock(int *descriptor);
void unlock(int *descriptor);
