#include <stdbool.h>

#include "exception/exception.h"
#include "page/page.h"
#include "printf/printf.h"
#include "process/process.h"
#include "thread/thread.h"
#include "timer/timer.h"
#include "uart/uart.h"

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
  timer_init();
  page_init();
  thread_init();
  process_init();
  uart_init();
  //
  printf("[INFO] test create\n");
  thread_create(test, NULL);
  //
  printf("[INFO] kernel idle\n");
  idle();
}
