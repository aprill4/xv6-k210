// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "kernel/include/types.h"
#include "kernel/include/stat.h"
#include "xv6-user/user.h"

void
getppid_test(void)
{
  int pid;

  printf("getppid_test test\n");

    pid = fork();

    if(pid == 0){
      printf("child, getppid: %d\n", getppid());
      printf("child, getpid: %d\n", getpid());
    }
    else{
      sleep(3);
      
      printf("parent, getppid: %d\n", getppid());
      printf("parent, getpid: %d\n", getpid());
    }

}

int
main(void)
{
  getppid_test();
  exit(0);
}
