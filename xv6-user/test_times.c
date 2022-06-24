#include "kernel/include/types.h"
#include "kernel/include/stat.h"
#include "xv6-user/user.h"

void
test_times(void)
{

  printf("testing times\n");

  printf("nothing to see here\n");
  {
    struct tms t;
    uint64 ret = times(&t);
    printf("times: %p\n", ret);
    printf("t->utime: %d\n", t.utime);
    printf("t->stime: %d\n", t.stime);
    printf("t->cutime: %d\n", t.cutime);
    printf("t->cstime: %d\n", t.cstime);
  }

  sleep(3);

  {
    struct tms t;
    uint64 ret = times(&t);
    printf("times: %p\n", ret);
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
