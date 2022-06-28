#ifndef __SIGNAL_H
#define __SIGNAL_H

#define SIGINT  2
#define SIGALRM 14

#define SIG_BIT(sig) (1ull << (sig))
#define SET_SIG(sig_mask, sig) ((sig_mask) |= SIG_BIT(sig))
#define CLR_SIG(sig_mask, sig) ((sig_mask) &= ~SIG_BIT(sig))
#define IS_SIG(sig_mask, sig) ((sig_mask) & SIG_BIT(sig))

#endif