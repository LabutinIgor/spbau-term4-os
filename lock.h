#pragma once

#include "threads.h"
#include "interrupt.h"

typedef struct lock_decsriptor {
    int is_locked;
} lock_decsriptor;

void lock(int *descriptor);
void unlock(int *descriptor);
