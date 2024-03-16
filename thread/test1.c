// 测试非主线程执行exit会发生什么，主线程是否会退出
// 测试结果：任何一个线程exit均会导致主线程退出，exit会调用exit_group和exit
#include <pthread.h>
#include <unistd.h>

void *test(void *arg) {
  exit(0);
}

int main(void) {
  pthread_t tid;
  pthread_create(&tid, NULL, test, NULL);
  while (1) {
    write(STDOUT_FILENO, "1\n", 2);
  }
}