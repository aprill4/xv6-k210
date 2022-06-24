#include "kernel/include/types.h"
#include "kernel/include/stat.h"
#include "xv6-user/user.h"

void
getmem_test(void)
{
  printf("getmem test\n");

  printf("getmem: %d\n", getmem());
}

int
main(void)
{
  getmem_test();
  exit(0);
}
