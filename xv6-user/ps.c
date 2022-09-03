#include "kernel/include/types.h"
#include "kernel/include/stat.h"
#include "xv6-user/user.h"

char buf[512];

void
cat(int fd)
{
  char *s = buf;
  int n;

  while((n = read(fd, s, sizeof(buf))) > 0) {
    s += n;
  }
  if(n < 0){
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

char*
fmtname(char *name)
{
  static char buf[STAT_MAX_NAME+1];
  int len = strlen(name);

  // Return blank-padded name.
  if(len >= STAT_MAX_NAME)
    return name;
  memmove(buf, name, len);
  memset(buf + len, ' ', STAT_MAX_NAME - len);
  buf[STAT_MAX_NAME] = '\0';
  return buf;
}

#define SKIP_SPACE(__str) do {while(*__str++ != ' ');} while(0)
void
ps_proc(const char *pid_str) {
  int fd;
  char stat_path[64] = {0};
  strcat(stat_path, "/proc/");
  strcat(stat_path, pid_str);
  strcat(stat_path, "/stat");

  if ((fd = open(stat_path, 0)) < 0) {
    fprintf(2, "ps: cannot open %s\n", stat_path);
    exit(1);
  }

  cat(fd);

  // parse stat fields
  char *s = buf;
  int pid = atoi(s);
  SKIP_SPACE(s); // skip pid
  SKIP_SPACE(s); // skip cmd
  char state = *s;
  SKIP_SPACE(s); // skip state
  int ppid = atoi(s);
  SKIP_SPACE(s); // skip ppid
  //int utime = atoi(s);
  SKIP_SPACE(s); // skip utime
  //int stime = atoi(s);
  SKIP_SPACE(s); // skip stime
  //int cutime = atoi(s);
  SKIP_SPACE(s); // skip cutime
  //int cstime = atoi(s);
  SKIP_SPACE(s); // skip cstime
  int sz = atoi(s);
  SKIP_SPACE(s); // skip sz

  printf("\t%d\t%d\t%s\t%c\t%d\t%d\t%d\n",
         pid, ppid, "(cmd)", state, 42, 42, sz);
  //printf("utime=%d, stime=%d, cutime=%d, cstime=%d\n", utime, stime, cutime, cstime);
}

void
ps() {
  int fd;
  struct stat st;

  // header
  printf("\tPID\tPPID\tCOMMAND\tS\tTIME\tELAPSED\tVSZ\n");

  if ((fd = open("/proc", 0)) < 0) {
    fprintf(2, "ps: cannot open `/proc`\n");
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "ps: cannot stat `/proc`\n");
    close(fd);
    return;
  }

  //assert (st.type == T_DIR)

  while(readdir(fd, &st) == 1){
    ps_proc(st.name);
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc >= 2){
    fprintf(2, "too many arguments\n");
    exit(1);
  }
  ps();
  exit(0);
}
