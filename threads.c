#include "threads.h"
#include "stdio.h"

typedef enum {RUNNING, JOINING, TERMINATED, DELETED} thread_state;

struct thread {
    void* stack;
    void* stack_pointer;
    thread_state state;
};

static pid_t cur_thread, cnt_threads;
static struct thread all_threads[THREADS_MAX_CNT];
static pid_t joins[THREADS_MAX_CNT];

void switch_threads(void **old_sp, void *new_sp);

void setup_threads() {
    cnt_threads = 1;
    cur_thread = 0;
    all_threads[0].state = RUNNING;
}

void scedule() {
    //printf("start shedule\n");
    pid_t new_thread = (cur_thread + 1) % cnt_threads;
    for (;;) {
        if (new_thread != cur_thread) {
            if (all_threads[new_thread].state == RUNNING) {
                pid_t old_thread = cur_thread;
                cur_thread = new_thread;

                //printf("switch %d to %d\n", old_thread, new_thread);
                
                switch_threads(&all_threads[old_thread].stack_pointer, all_threads[cur_thread].stack_pointer);
                
                //printf("returned to scedule\n");

                break;
            }
            if (all_threads[new_thread].state == TERMINATED) {
                kmem_free(all_threads[new_thread].stack);
                all_threads[new_thread].state = DELETED;
            }
        } else {
            if (all_threads[new_thread].state == RUNNING) {
                //printf("same thread\n");
                break;
            }
        }
        new_thread = (new_thread + 1) % cnt_threads;
    }
}

void exit_thread() {
    local_irq_disable();
    all_threads[cur_thread].state = TERMINATED;

    if (all_threads[joins[cur_thread]].state == JOINING) {
        all_threads[joins[cur_thread]].state = RUNNING;
    }
    //printf("exited %d\n", cur_thread);
    scedule();
    //printf("WTF?!\n");
}

void join_thread(pid_t join_thread) {
    local_irq_disable();
    all_threads[cur_thread].state = JOINING;
    joins[join_thread] = cur_thread;
    while (all_threads[join_thread].state != TERMINATED && all_threads[join_thread].state != DELETED) {
        //printf("scedule from join\n");
        scedule();
        //printf("end join schedule\n");
    }
    local_irq_enable();
}

pid_t create_thread(void (*fptr)(void *), void *arg) {
    local_irq_disable();
    cnt_threads++;
    struct thread *new_thread = &all_threads[cnt_threads - 1];

    new_thread->stack = kmem_alloc(PAGE_SIZE * 2);
    new_thread->stack_pointer = (uint8_t*) new_thread->stack + PAGE_SIZE * 2;

    struct thread_data  {
        uint64_t r15, r14, r13, r12, rbx, rbp;
        void* start_thread_addr;
        void (*fptr)(void*);
        void* arg;
    };

    new_thread->stack_pointer = (uint8_t*) new_thread->stack_pointer - sizeof(struct thread_data);

    struct thread_data* init_val = new_thread->stack_pointer;

    extern void *start_thread;

    init_val->start_thread_addr = &start_thread;

    init_val->r12 = 0;
    init_val->r13 = 0;
    init_val->r14 = 0;
    init_val->r15 = 0;
    init_val->rbx = 0;
    init_val->rbp = 0;

    init_val->fptr = fptr;
    init_val->arg = arg;

    new_thread->state = RUNNING;

    local_irq_enable();

    return (pid_t) (cnt_threads - 1);
}
