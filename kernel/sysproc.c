
#include "include/types.h"
#include "include/riscv.h"
#include "include/param.h"
#include "include/memlayout.h"
#include "include/spinlock.h"
#include "include/proc.h"
#include "include/syscall.h"
#include "include/timer.h"
#include "include/kalloc.h"
#include "include/string.h"
#include "include/printf.h"
#include "include/timer.h"

extern int exec(char *path, char **argv);

uint64
sys_exec(void)
{
  char path[FAT32_MAX_PATH], *argv[MAXARG];
  int i;
  uint64 uargv, uarg;

  if(argstr(0, path, FAT32_MAX_PATH) < 0 || argaddr(1, &uargv) < 0){
    return -1;
  }
  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv)){
      goto bad;
    }
    if(fetchaddr(uargv+sizeof(uint64)*i, (uint64*)&uarg) < 0){
      goto bad;
    }
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    argv[i] = kalloc();
    if(argv[i] == 0)
      goto bad;
    if(fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  int ret = exec(path, argv);

  for(i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  return ret;

 bad:
  for(i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  return -1;
}

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getppid(void)
{
  return myproc()->parent->pid;
}

uint64
sys_getmem(void)
{
  return myproc()->sz / 1024;
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int mask;
  if(argint(0, &mask) < 0) {
    return -1;
  }
  myproc()->tmask = mask;
  return 0;
}

uint64
sys_times(void) {
  
  uint64 p;
  if(argaddr(0, &p) < 0) {
    return -1;
  }

  if (either_copyout(1, p, &myproc()->proc_tms, sizeof(struct tms)) < 0) {
    return -1;
  }

  return r_time();
}

uint64
sys_alarm(void) {
  int seconds;
  if(argint(0, &seconds) < 0) {
    return -1;
  }

  if(seconds < 0) {
    return -1;
  }

  //uint64 sched_alarm = r_time() + CLOCKFREQ * seconds;
  uint64 sched_alarm = ticks + seconds * TICKS_PER_SEC;
  struct proc *p = myproc();
  acquire(&p->lock);
  p->sched_alarm = sched_alarm;
  release(&p->lock);

  // FIXME: Return the number of seconds remaining until any
  // previously scheduled alarm was due to be delivered, or zero if
  // there was no previously scheduled alarm.
  return -1;
}

void
sys_pause(void)
{
  struct proc *p = myproc();

  acquire(&p->sig_lock);
  while(p->sig == 0){
    if(myproc()->killed){
      return;
    }
    sleep(&p->sig, &p->sig_lock);
  }
  release(&p->sig_lock);
}

uint64
sys_signal(void) {
  uint sig;
  uint64 addr;

  if (arguint(0, &sig) < 0) {
    return 0;
  }

  if (argaddr(1, &addr) < 0) {
    return 0;
  }

  if (sig == 0 || sig > NSIG) {
    panic("signal: invalid signal number"); 
  }

  // FIXME: Do we need locks?
  struct proc *p = myproc();
  sig_handler *old = p->sig_actions[sig - 1];

  printf("signal: sig %d, addr %p\n", sig, addr);
  p->sig_actions[sig - 1] = (sig_handler *)addr;

  return (uint64)old;
}

void
sys_rt_sigreturn(void) {
  struct proc *p = myproc();
  p->trapframe = p->sig_frame.old_tf;
  // FIXME: Assert that proc will continue running
}