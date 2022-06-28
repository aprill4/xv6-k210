#include "kernel/include/types.h"
#include "kernel/include/stat.h"
#include "xv6-user/user.h"

void
test_alarm(void)
{

  printf("testing alarm\n");

  printf("----------------------------\n");
  {
    struct tms t;
    uint64 ret = times(&t);
    printf("times: %d\n", ret);
    printf("t->utime: %d\n", t.utime);
    printf("t->stime: %d\n", t.stime);
    printf("t->cutime: %d\n", t.cutime);
    printf("t->cstime: %d\n", t.cstime);
  }
  printf("----------------------------\n");

  alarm(3);

  printf("going to sleep, see you 3 seconds later\n");
  pause();
  //while (1) {}
  printf("now reachable\n");

}

int
main(void)
{
  test_alarm();
  exit(0);
}
