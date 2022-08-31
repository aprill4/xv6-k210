# trampoline for signal handling

#include "include/sysnum.h"

.section sigtrampolinesec
.globl sig_trampoline

sig_trampoline:
.globl sig_handler_wrapper
# void sig_handler(int signum, sig_handler *handler);
sig_handler_wrapper:
    jalr a1
    li a7, SYS_rt_sigreturn
    ecall

.globl default_signal_handler
default_signal_handler:
    li a0, -1
    li a7, SYS_exit