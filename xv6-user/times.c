#include "kernel/include/types.h"
#include "kernel/include/stat.h"
#include "xv6-user/user.h"

void
test_times(void)
{

  printf("testing times\n");

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

  sleep(3);
  long long ticks = 0x100000000;
  while (ticks--) {
    asm volatile("nop");
  }

  {
    struct tms t;
    uint64 ret = times(&t);
    printf("times: %d\n", ret);
    printf("t->utime: %d\n", t.utime);
    printf("t->stime: %d\n", t.stime);
    printf("t->cutime: %d\n", t.cutime);
    printf("t->cstime: %d\n", t.cstime);
  }

}

int
main(void)
{
  test_times();
  exit(0);
}
