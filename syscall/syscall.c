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

int exec(char* name, char** argv) {
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

unsigned open(char* pathname, unsigned flags) {
  struct syscall_open context = {
      .pathname = pathname, .flags = flags, .result = 0};
  asm("mov x0, %0" ::"r"(&context));
  asm("svc 6");
  return context.result;
}

void close(unsigned fd) {
  asm("mov x0, %0" ::"r"(&fd));
  asm("svc 7");
}

unsigned read(unsigned fd, unsigned size, void* buffer) {
  struct syscall_read context = {
      .fd = fd, .size = size, .buffer = buffer, .result = 0};
  asm("mov x0, %0" ::"r"(&context));
  asm("svc 8");
  return context.result;
}

unsigned write(unsigned fd, unsigned size, void* buffer) {
  struct syscall_write context = {
      .fd = fd, .size = size, .buffer = buffer, .result = 0};
  asm("mov x0, %0" ::"r"(&context));
  asm("svc 9");
  return context.result;
}
