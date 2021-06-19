#include <stdbool.h>

#include "app/app.h"
#include "cpio/cpio.h"
#include "exception/exception.h"
#include "page/page.h"
#include "printf/printf.h"
#include "thread/thread.h"
#include "timer/timer.h"
#include "uart/uart.h"

#define INITFS_ADDR (void*)0x8000000

void foo() {
  for (unsigned i = 0; i < 10; i++) {
    printf("thread: %d, i: %d\n", thread_current(), i);
    for (unsigned j = 0; j < 100000000; j++) {
      asm("nop");
    }
    thread_yield();
  }
}

int main() {
  printf("[Init Kernel]\n");
  exception_init();
  uart_init();
  page_init();
  timer_init();
  thread_init();
  printf("[Setup Thread]\n");
  for (unsigned i = 0; i < 5; i++) {
    thread_create(foo);
  }
  printf("[Begin Thread]\n");
  thread_yield();
  return 0;
}
