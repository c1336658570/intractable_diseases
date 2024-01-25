// 启动内存泄漏检测的话需要多传递一个参数
// 代码存在的问题：不能检测double free，只能检测内存泄漏
// gcc 05memleakv3.c -ldl
// ./a.out
// ./a.out -mem
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int memleak_enable = 0;

typedef void *(*malloc_t)(size_t size);
malloc_t malloc_f = NULL;

typedef void (*free_t)(void *ptr);
free_t free_f = NULL;

int enable_malloc_hook = 1;
int enable_free_hook = 1;

void *malloc(size_t size) {
  void *ptr = NULL;
  
  // enable_malloc_hook用来防止malloc的递归调用，因为如果malloc内部有函数调用隐式调用malloc(比如printf)就会导致递归调用
  if (enable_malloc_hook) {
    enable_malloc_hook = 0;

    ptr = malloc_f(size);

    if (memleak_enable) {
      void *caller = __builtin_return_address(0);

      char buff[128] = {0};
      snprintf(buff, 128, "./mem/%p", ptr);

      FILE *fp = fopen(buff, "w");
      fprintf(fp, "[+] caller: %p, addr: %p, size: %ld\n", caller, ptr, size);
      fflush(fp);
      fclose(fp);
    }
    enable_malloc_hook = 1;
  } else {
    ptr = malloc_f(size);
  }

  return ptr;
}

void free(void *ptr) {
  // enable_free_hook用来防止free递归调用，即free内部调用的函数，隐式调用了free
  if (enable_free_hook) {
    enable_free_hook = 0;
    if (memleak_enable) {
      char buff[128] = {0};
      snprintf(buff, 128, "./mem/%p", ptr);
      /*
      // 此处的double free检测有问题，因为malloc函数中的fopen等函数都会调用malloc分配内存，然后fclose会调用free释放内存
      // 会造成fopen调用的free会进入if的这个分支，然后导致double free，其实根本没有double free
      if (unlink(buff) < 0) {
        printf("double free\n");
        return;
      }
      */
      unlink(buff);
    }
    free_f(ptr);

    enable_free_hook = 1;
  } else {
    free_f(ptr);
  }
  return;
}

void init_hook(void) {
  if (!malloc_f) {
    malloc_f = dlsym(RTLD_NEXT, "malloc");
  }
  if (!free_f) {
    free_f = dlsym(RTLD_NEXT, "free");
  }
}

int main(int argc, char *argv) {
  init_hook();

  if (argc >= 2) {
    memleak_enable = 1;
  }

  int *p1 = malloc(4);
  // int *p2 = malloc(4);
  int *p3 = malloc(4);

  free(p1);
  free(p3);
}
