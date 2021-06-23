#include <stdbool.h>

#include "exception/exception.h"
#include "page/page.h"
#include "printf/printf.h"
#include "process/process.h"
#include "thread/thread.h"
#include "timer/timer.h"
#include "tmpfs/tmpfs.h"
#include "uart/uart.h"
#include "vfs/vfs.h"

void test() {
  char* argv[] = {"argv_test", "-o", "arg2", NULL};
  process_run("argv_test", argv);
}

void idle() {
  while (true) {
    thread_clean();
    thread_yield();
  }
}

void main() {
  printf("[INFO] kernel init\n");
  exception_init();
  page_init();
  process_init();
  thread_init();
  timer_init();
  tmpfs_init();
  uart_init();
  printf("[INFO] kernel run tmpfs_test");
  process_run("tmpfs_test", NULL);
  printf("[INFO] kernel idle\n");
  idle();
}
