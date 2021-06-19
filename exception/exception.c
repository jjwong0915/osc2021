#include "exception/exception.h"

#include <stddef.h>
#include <stdint.h>

#include "app/app.h"
#include "page/page.h"
#include "printf/printf.h"
#include "timer/timer.h"

void exception_init() {
  asm("msr hcr_el2, %0" ::"r"(1 << 31));
  asm("msr spsr_el2, %0" ::"r"(0x3c5));
  // setup vector table
  void* vector_address = NULL;
  asm("ldr %0, =exception_vector" : "=r"(vector_address));
  asm("msr vbar_el1, %0" ::"r"(vector_address));
  // setup stack pointer
  void* sp_address = NULL;
  asm("mov %0, sp" : "=r"(sp_address));
  asm("msr sp_el1, %0" ::"r"(sp_address));
  // setup execution level
  void* lr_address = NULL;
  asm("ldr %0, =exception_init_end" : "=r"(lr_address));
  asm("msr elr_el2, %0" ::"r"(lr_address));
  asm("eret");
  asm("exception_init_end:");
}

void exception_handle_default() {
  printf("<Exception Enter>\n");
  //
  uint64_t spsr_el1 = 0;
  asm("mrs %0, spsr_el1" : "=r"(spsr_el1));
  printf("spsr_el1 %x\n", spsr_el1);
  uint64_t elr_el1 = 0;
  asm("mrs %0, elr_el1" : "=r"(elr_el1));
  printf("elr_el1 %x\n", elr_el1);
  uint64_t esr_el1 = 0;
  asm("mrs %0, esr_el1" : "=r"(esr_el1));
  printf("esr_el1 %x\n", esr_el1);
  // check if the exception is caused by a system call
  if (((esr_el1 >> 26) & 0x3f) == 0x15) {
    unsigned syscall_number = esr_el1 & 0x1fffff;
    printf("syscall_number %d\n", syscall_number);
    if (syscall_number == 0) {
      app_exit();
    }
  }
  //
  printf("<Exception End>\n");
}

void exception_handle_irq() {
  printf("<Exception Enter>\n");
  //
  uint64_t cntpct_el0 = 0;
  asm("mrs %0, cntpct_el0" : "=r"(cntpct_el0));
  uint64_t cntfrq_el0 = 0;
  asm("mrs %0, cntfrq_el0" : "=r"(cntfrq_el0));
  printf("seconds after boot: %lu\n", cntpct_el0 / cntfrq_el0);
  //
  timer_reset();
  //
  printf("<Exception End>\n");
}
