#include "exception/exception.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "page/page.h"
#include "printf/printf.h"
#include "process/process.h"
#include "program/program.h"
#include "syscall/syscall.h"
#include "timer/timer.h"
#include "uart/uart.h"

static void handle_syscall(unsigned number, void* context) {
  if (number == 0) {
    *(unsigned*)context = process_current();
  } else if (number == 1) {
    uart_send(*(unsigned int*)context);
  } else if (number == 2) {
    *(char*)context = uart_getc();
  } else if (number == 3) {
    struct syscall_exec* exec_ctx = (struct syscall_exec*)context;
    int (*func)(int, char* const*) = program_load(exec_ctx->name);
    exec_ctx->result = func(program_argc(exec_ctx->argv), exec_ctx->argv);
  } else if (number == 4) {
    process_exit();
  } else if (number == 5) {
    *(unsigned*)context = process_fork();
  }
}

void exception_init() {
  asm("msr hcr_el2, %0" ::"r"(1 << 31));
  asm("msr spsr_el2, %0" ::"r"(0x345));
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
  // backup the first argument
  void* context = NULL;
  asm("mov %0, x0" : "=r"(context));
  // check if the exception is caused by a system call
  uint64_t esr_el1 = 0;
  asm("mrs %0, esr_el1" : "=r"(esr_el1));
  if (((esr_el1 >> 26) & 0x3f) == 0x15) {
    unsigned number = esr_el1 & 0x1fffff;
    handle_syscall(number, context);
  } else {
    uint64_t spsr_el1 = 0, elr_el1 = 0;
    asm("mrs %0, spsr_el1" : "=r"(spsr_el1));
    asm("mrs %0, elr_el1" : "=r"(elr_el1));
    printf("[ERROR] exception occured\n");
    printf("[INFO] spsr_el1: 0x%x\n", spsr_el1);
    printf("[INFO] elr_el1: 0x%x\n", elr_el1);
    printf("[INFO] esr_el1: 0x%x\n", esr_el1);
    // pause execution
    while (true) {
      asm("nop");
    }
  }
}

void exception_handle_irq() {
  uint64_t cntpct_el0 = 0;
  asm("mrs %0, cntpct_el0" : "=r"(cntpct_el0));
  uint64_t cntfrq_el0 = 0;
  asm("mrs %0, cntfrq_el0" : "=r"(cntfrq_el0));
  printf("[INFO] seconds after boot: %lu\n", cntpct_el0 / cntfrq_el0);
  timer_reset();
}
