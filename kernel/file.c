//
// Support functions for system calls that involve file descriptors.
//


#include "include/types.h"
#include "include/riscv.h"
#include "include/param.h"
#include "include/spinlock.h"
#include "include/sleeplock.h"
#include "include/fat32.h"
#include "include/file.h"
#include "include/pipe.h"
#include "include/stat.h"
#include "include/proc.h"
#include "include/printf.h"
#include "include/string.h"
#include "include/vm.h"

struct devsw devsw[NDEV];
struct {
  struct spinlock lock;
  struct file file[NFILE];
} ftable;

void
fileinit(void)
{
  initlock(&ftable.lock, "ftable");
  struct file *f;
  for(f = ftable.file; f < ftable.file + NFILE; f++){
    memset(f, 0, sizeof(struct file));
  }
  #ifdef DEBUG
  printf("fileinit\n");
  #endif
}

// Allocate a file structure.
struct file*
filealloc(void)
{
  struct file *f;

  acquire(&ftable.lock);
  for(f = ftable.file; f < ftable.file + NFILE; f++){
    if(f->ref == 0){
      f->ref = 1;
      release(&ftable.lock);
      return f;
    }
  }
  release(&ftable.lock);
  return NULL;
}

// Increment ref count for file f.
struct file*
filedup(struct file *f)
{
  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  return f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  struct file ff;

  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("fileclose");
  if(--f->ref > 0){
    release(&ftable.lock);
    return;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  if(ff.type == FD_PIPE){
    pipeclose(ff.pipe, ff.writable);
  } else if(ff.type == FD_ENTRY){
    eput(ff.ep);
  } else if (ff.type == FD_DEVICE) {

  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
int
filestat(struct file *f, uint64 addr)
{
  // struct proc *p = myproc();
  struct stat st;
  
  if (f->ep->proc_vfs_type == PROC_VFS_PID_STAT) {
    strncpy(st.name, "stat", 5);
    st.dev = 0;
    st.type = T_FILE;
    st.size = 0;
  } else if(f->type == FD_ENTRY){
    elock(f->ep);
    estat(f->ep, &st);
    eunlock(f->ep);
  }
  // if(copyout(p->pagetable, addr, (char *)&st, sizeof(st)) < 0)
  if(copyout2(addr, (char *)&st, sizeof(st)) < 0)
    return -1;
  return 0;
}

// Read from file f.
// addr is a user virtual address.
int
fileread(struct file *f, uint64 addr, int n)
{
  int r = 0;

  if(f->readable == 0)
    return -1;

  if (f->ep->proc_vfs_type == PROC_VFS_PID_STAT) {
    if (f->off == 0) {
      return 0;
    }
    f->off--;
    
    struct proc *p = findproc(f->ep->pid);

    if (p == NULL) {
      panic("fileread: pid is invalid");
    }

    char state = "USRRZ"[p->state];

    uint64 utime = p->proc_tms.utime + p->proc_tms.cutime;
    uint64 stime = p->proc_tms.stime + p->proc_tms.cstime;
    int utime_sec = utime / CLOCKFREQ;
    int stime_sec = stime / CLOCKFREQ;
    //printf("dbg: utime_sec=%d, stime_sec=%d\n", utime_sec, stime_sec);

    char line[128];
    int ppid = (p->parent == NULL ? -1 : p->parent->pid);
    sprintf(line, "%d (cmd) %c %d %d %d %d %d utime:%d, stime:%d\n", p->pid, state, ppid,
            p->proc_tms.utime, p->proc_tms.stime, p->proc_tms.cutime,
            p->proc_tms.cstime, p->sz, utime_sec, stime_sec);
    int len = strlen(line);
    if (n < len) {
      panic("reading part of proc stat");
    }
    copyout2(addr, line, len);
    r = len;
  } else {
    switch (f->type) {
      case FD_PIPE:
          r = piperead(f->pipe, addr, n);
          break;
      case FD_DEVICE:
          if(f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
            return -1;
          r = devsw[f->major].read(1, addr, n);
          break;
      case FD_ENTRY:
          elock(f->ep);
            if((r = eread(f->ep, 1, addr, f->off, n)) > 0)
              f->off += r;
          eunlock(f->ep);
          break;
      default:
        panic("fileread");
    }
  }

  return r;
}

// Write to file f.
// addr is a user virtual address.
int
filewrite(struct file *f, uint64 addr, int n)
{
  int ret = 0;

  if(f->writable == 0)
    return -1;

  if(f->type == FD_PIPE){
    ret = pipewrite(f->pipe, addr, n);
  } else if(f->type == FD_DEVICE){
    if(f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      return -1;
    ret = devsw[f->major].write(1, addr, n);
  } else if(f->type == FD_ENTRY){
    elock(f->ep);
    if (ewrite(f->ep, 1, addr, f->off, n) == n) {
      ret = n;
      f->off += n;
    } else {
      ret = -1;
    }
    eunlock(f->ep);
  } else {
    panic("filewrite");
  }

  return ret;
}

// Read from dir f.
// addr is a user virtual address.
int
dirnext(struct file *f, uint64 addr)
{
  // struct proc *p = myproc();

  if(f->readable == 0 || !(f->ep->attribute & ATTR_DIRECTORY))
    return -1;

  struct dirent de;
  struct stat st;

  // May you rest in dreamless slumber
  if (f->ep->proc_vfs_type != PROC_VFS_INVALID) {
    switch (f->ep->proc_vfs_type) {
      case PROC_VFS_PROC_ROOT: {

        if (f->off == 0) {
          return 0;
        }

        int pid = getproc(--(f->off))->pid;
        itoa(pid, st.name);
        st.dev = 0;
        st.type = T_DIR;
        st.size = 0;

      } break;

      case PROC_VFS_PID_DIR: {

        if (f->off-- == 0) {
          return 0;
        }

        strncpy(st.name, "stat", 5);
        st.dev = 0;
        st.type = T_FILE;
        st.size = 0;

      } break;

      default: panic("unhandled proc vfs type");
    }
  } else {
    int count = 0;
    int ret;
    elock(f->ep);
    while ((ret = enext(f->ep, &de, f->off, &count)) == 0) {  // skip empty entry
      f->off += count * 32;
    }
    eunlock(f->ep);
    if (ret == -1)
      return 0;

    f->off += count * 32;
    estat(&de, &st);
  }

  // if(copyout(p->pagetable, addr, (char *)&st, sizeof(st)) < 0)
  if(copyout2(addr, (char *)&st, sizeof(st)) < 0)
    return -1;

  return 1;
}