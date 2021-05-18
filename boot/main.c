#include <stdbool.h>

#include "app/app.h"
#include "cpio/cpio.h"
#include "exception/exception.h"
#include "page/page.h"
#include "printf/printf.h"
#include "timer/timer.h"
#include "uart/uart.h"

#define INITFS_ADDR (void*)0x8000000

int main() {
  printf("[Init Kernel]\n");
  exception_init();
  uart_init();
  page_init();
  timer_init();
  printf("[Load Program]\n");
  struct cpio_file executable = cpio_open(INITFS_ADDR, "hello");
  if (executable.address == NULL) {
    printf("program file not found\n");
    return 1;
  }
  void* program = page_alloc(4);
  for (unsigned i = 0; i < executable.size; i++) {
    *(char*)(program + i) = *(char*)(executable.address + i);
  }
  printf("[Begin Program]\n");
  int status = app_execute(program);
  printf("[End Program] status=%d\n", status);
  return 0;
}
