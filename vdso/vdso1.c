// vdso用来加速系统调用，通过映射到用户空间的代码和数据来模拟系统调用，来达到加速的目的。
// ldd /bin/ls    可以看到有	linux-vdso.so.1 (0x00007ffcdeac2000)
// 虽然没有对应的磁盘文件，在文件系统中找不到，但是操作系统在启动进程时已经将其作为虚拟共享库加载过，
// 所以再次使用 dlopen 时能够正确找到该共享库，然后再使用 dlsym 找到对应的导出函数即可。

// cat /proc/self/maps    也可以看到vdso，还有vdso的地址，根据地质将vdsodump出来
// 7ffff7fc3000-7ffff7fc5000 r-xp 00000000 00:00 0                          [vdso]

// 将vdso给dump出来，可以file vdso.so

// 下载gdb-peda插件，然后设置为每次启动gdb都会加载
// git clone git@github.com:longld/peda.git ~/peda
// echo "source ~/peda/peda.py" >> ~/.gdbinit
// 通过gdb运行，并把vdso那段内存给dump到文件里
// gdb ./memory
// dump mem vdso.so 0x7ffff7fc3000 0x7ffff7fc5000
// 查看文件类型
// file vdso.so                                 2m 40s 18:00:31
// vdso.so: ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked, BuildID[sha1]=f28f2e23e192f4e703ed7dfe0e44462364accc91, stripped
// 查看它的导出函数
// objdump -T vdso.so 


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
