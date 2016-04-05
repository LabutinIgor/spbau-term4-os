.code64
.section .text
.globl start_thread
.globl switch_threads

start_thread:
    popq %rax
    popq %rdi
    callq *%rax
    callq exit_thread

// int switch_threads(void **old_sp, void *new_sp); // C signature return previous thread
switch_threads:
    pushq %rbp        // save volatile registers
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    movq %rsp, (%rdi) // save SP
    movq %rsi, %rsp   // restore SP

    popq %r15         // restore volatile register
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp

    ret
