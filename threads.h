#pragma once

#include <sys/types.h>
#include "interrupt.h"
#include "memory.h"
#include "kmem_cache.h"
#include "assert.h"
#include "lock.h"

#define THREADS_MAX_CNT 1000

void setup_threads();
pid_t create_thread(void (*fptr)(void *), void *arg);
void scedule();
pid_t get_current_thread();
void join_thread(pid_t thread);
