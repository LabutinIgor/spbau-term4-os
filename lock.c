#include "lock.h"

void lock(int *descriptor)
{
    local_irq_disable();
    while (*descriptor == 1) {
        scedule();
    }
    *descriptor = 1;
    local_irq_enable();
}

void unlock(int *descriptor)
{
    local_irq_disable();
    *descriptor = 0;
    local_irq_enable();
}
