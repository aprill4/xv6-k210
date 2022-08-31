#ifndef __SIGNAL_H
#define __SIGNAL_H

#include "trap.h"

#define SIGINT   1
#define SIGALRM  2
#define NSIG     2

#define SIG_DFL  (sig_handler *)(0)
#define SIG_IGN  (sig_handler *)(0xffffffffffffffffULL)

typedef void sig_handler(uint);

#define SIG_BIT(sig) (1ull << (sig))
#define SET_SIG(sig_mask, sig) ((sig_mask) |= SIG_BIT(sig))
#define CLR_SIG(sig_mask, sig) ((sig_mask) &= ~SIG_BIT(sig))
#define IS_SIG(sig_mask, sig) ((sig_mask) & SIG_BIT(sig))

struct sig_frame {
    struct trapframe *old_tf;
    struct trapframe *tf;
};

#endif