// 启动内存泄漏检测的话需要打开宏，编译指令如下：
// gcc -Dmemleak_enable 04memleakv1.c
// ./a.out
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

# ifdef memleak_enable

void *_malloc(size_t size, const char *filename, int line) {
  void *ptr = malloc(size);
  if (memleak_enable) {
    char buff[64] = {0};
    snprintf(buff, 64, "./mem/%p", ptr);

    FILE *fp = fopen(buff, "w");
    fprintf(fp, "[+] mem_addr: %p, filename: %s, line: %d\n", ptr, filename, line);
    fflush(fp);
    fclose(fp);
  }

  return ptr;
}

void _free(void *ptr, const char *filename, int line) {
  if (memleak_enable) {
    char buff[64] = {0};
    snprintf(buff, 64, "./mem/%p", ptr);
    if (unlink(buff) < 0) {
      printf("double free\n");
      return;
    }
  }
  return free(ptr);
}

#define malloc(size) _malloc(size, __FILE__, __LINE__)
#define free(ptr) _free(ptr, __FILE__, __LINE__)

#endif

int main(void) {
  int *p1 = malloc(4);
  int *p2 = malloc(4);
  int *p3 = malloc(4);

  free(p1);
  free(p3);
}
