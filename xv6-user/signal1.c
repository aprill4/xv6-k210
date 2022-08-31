#include "kernel/include/types.h"
#include "kernel/include/stat.h"
#include "kernel/include/signal.h"
#include "xv6-user/user.h"

void
panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

void
test_signal(void)
{

  printf("testing signal dft\n");

  printf("setting signal handler to SIG_DFL\n");
  signal(SIGALRM, SIG_DFL);

  printf("calling alarm\n");
  alarm(3);

  printf("going to sleep, see you 3 seconds later\n");
  pause();

  panic("unreachable");
}

int
main(void)
{
  test_signal();
  exit(0);
}
