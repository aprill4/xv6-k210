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

  alarm(1);

  while (1) {}
  printf("unreachable\n");

}

int
main(void)
{
  test_alarm();
  exit(0);
}
