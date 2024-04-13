// 因为 vDSO 是一个比较标准的共享动态链接库，所以也可以使用 dlopen 打开它
// 通过dlopen打开动态链接库，dlsym找到__vdso_gettimeofday函数的地址

#include <stdio.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <unistd.h>

void *get_vdso_sym(const char *name) {
  void *handle;
  void *sym;

  // 尝试打开 vDSO，RTLD_NOW 立即解析所有符号，RTLD_GLOBAL 允许后续打开的库引用这些符号  
  handle = dlopen("linux-vdso.so.1", RTLD_NOW | RTLD_GLOBAL);
  if (handle) {
    (void)dlerror();
    sym = dlsym(handle, name);    // 尝试获取符号
    if (dlerror()) sym = NULL;
  } else {
    sym = NULL;
  }

  return sym;
}

typedef int(gettimeofday_t)(struct timeval *tv, struct timezone *tz);

int main() {
  gettimeofday_t *my_gettimeofday = (gettimeofday_t *)get_vdso_sym("__vdso_gettimeofday");
  struct timeval tv;
  struct timezone tz;

  my_gettimeofday(&tv, &tz);
  printf("tv_sec=%ld, tv_usec=%ld\n", tv.tv_sec, tv.tv_usec);
  
  return 0;
}