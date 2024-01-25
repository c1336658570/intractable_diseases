// 启动内存泄漏检测的话需要多传递一个参数
// ./a.out -mem
#include <stdio.h>
#include <stdlib.h>

int memleak_enable = 0;

void *_malloc(size_t size, const char *filename, int line) {
  void *ptr = malloc(size);
  if (memleak_enable) {
    printf("[+] mem_addr: %p, filename: %s, line: %d\n", ptr, filename, line);
  }

  return ptr;
}

void _free(void *ptr, const char *filename, int line) {
  if (memleak_enable) {
    printf("[-] mem_addr: %p, filename: %s, line: %d\n", ptr, filename, line);
  }
  return free(ptr);
}

#define malloc(size) _malloc(size, __FILE__, __LINE__)
#define free(ptr) _free(ptr, __FILE__, __LINE__)

int main(int argc, char *argv) {
  if (argc >= 2) {
    memleak_enable = 1;
  }

  int *p1 = malloc(4);
  int *p2 = malloc(4);
  int *p3 = malloc(4);

  free(p1);
  free(p3);
}
