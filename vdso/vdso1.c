// vdso用来加速系统调用，通过映射到用户空间的代码和数据来模拟系统调用，来达到加速的目的。
// ldd /bin/ls    可以看到有	linux-vdso.so.1 (0x00007ffcdeac2000)
// 虽然没有对应的磁盘文件，在文件系统中找不到，但是操作系统在启动进程时已经将其作为虚拟共享库加载过，
// 所以再次使用 dlopen 时能够正确找到该共享库，然后再使用 dlsym 找到对应的导出函数即可。

// cat /proc/self/maps    也可以看到vdso

// gcc vdso1.c -o vdso1

// ldd vdso1
// linux-vdso.so.1 (0x00007ffd24794000)

// strace ./vdso1 2>&1 | grep gettiemofday
// 没有追踪到gettiemofday，说明gettimeofday被vdso优化了


#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
  struct timeval tv;
  struct timezone tz;

  gettimeofday(&tv, &tz);
  printf("tv_sec=%ld, tv_usec=%ld\n", tv.tv_sec, tv.tv_usec);

  return 0;
}
