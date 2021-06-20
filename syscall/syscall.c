#include "syscall/syscall.h"

unsigned getpid() {
  unsigned result = 0;
  asm("mov x0, %0" ::"r"(&result));
  asm("svc 0");
  return result;
}

void uart_send(unsigned int c) {
  asm("mov x0, %0" ::"r"(&c));
  asm("svc 1");
}

char uart_getc() {
  char result = '\0';
  asm("mov x0, %0" ::"r"(&result));
  asm("svc 2");
  return result;
}

int exec(char* name, char* const argv[]) {
  struct syscall_exec context = {.name = name, .argv = argv, .result = 0};
  asm("mov x0, %0" ::"r"(&context));
  asm("svc 3");
  return context.result;
}

void exit() { asm("svc 4"); }

unsigned fork() {
  unsigned result = 0;
  asm("mov x0, %0" ::"r"(&result));
  asm("svc 5");
  return result;
}
