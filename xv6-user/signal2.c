#include "kernel/include/types.h"
#include "kernel/include/stat.h"
#include "xv6-user/user.h"

void
panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

void ding(uint sig) {
  printf("ding\n");
}

void
test_signal(void)
{

  printf("testing signal: custom handler\n");

  printf("setting signal handler to `ding`\n");
  printf("addr of ding: %p\n", &ding);
  printf("addr of panic: %p\n", &panic);
  signal(SIGALRM, &ding);

  printf("calling alarm\n");
  alarm(3);

  printf("going to sleep, see you 3 seconds later\n");
  pause();

  printf("hello again\n");
}

int
main(void)
{
  test_signal();
  exit(0);
}
