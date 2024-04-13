// gcc vdso2.c -o vdso2
// strace ./vdso2 2>&1 | grep gettiemofday
// 此处也没有追踪到gettiemofday，可能系统调用直接被优化了

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>

int main() {
  struct timeval tv;
  struct timezone tz;

  syscall(SYS_gettimeofday, &tv, &tz);
  printf("tv_sec=%ld, tv_usec=%ld\n", tv.tv_sec, tv.tv_usec);

  return 0;
}