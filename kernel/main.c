#include <stdbool.h>

#include "exception/exception.h"
#include "fat32/fat32.h"
#include "page/page.h"
#include "printf/printf.h"
#include "process/process.h"
#include "thread/thread.h"
#include "timer/timer.h"
#include "tmpfs/tmpfs.h"
#include "uart/uart.h"
#include "vfs/vfs.h"

void idle() {
  while (true) {
    thread_clean();
    thread_yield();
  }
}

void main() {
  uart_init();
  printf("[INFO] kernel init\n");
  exception_init();
  fat32_init();
  page_init();
  process_init();
  thread_init();
  timer_init();
  printf("[INFO] kernel test\n");
  struct vfs_file* file = vfs_open("START.ELF", 0);
  char buffer[32];
  vfs_read(file, 32, buffer);
  for (unsigned i = 0; i < 32; i++) {
    printf("%2x ", buffer[i]);
  }
  printf("\n");
  printf("[INFO] kernel idle\n");
  idle();
}
