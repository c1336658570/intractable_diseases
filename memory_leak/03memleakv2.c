// 启动内存泄漏检测的话需要多传递一个参数
// ./a.out -mem
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int memleak_enable = 0;

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
