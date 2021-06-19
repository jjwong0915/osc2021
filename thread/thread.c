#include "thread/thread.h"

#include <stdbool.h>
#include <stdint.h>

#include "page/page.h"
#include "printf/printf.h"
#define THREAD_MAX 1024

struct thread {
  void* stack;
  uint64_t saved[11];
  uint64_t lr;
  uint64_t sp;
  //
  unsigned next;
};

struct thread thread_array[THREAD_MAX];
static unsigned thread_end;

static void idle(void) {
  while (true) {
    thread_yield();
  }
}

void thread_init() {
  thread_create(NULL);  // current thread
  asm("msr tpidr_el1, %0" ::"r"(0));
  thread_create(idle);
}

void thread_create(void (*func)(void)) {
  if (thread_end >= THREAD_MAX) {
    return UINT32_MAX;
  }
  void* stack = page_alloc(0);
  thread_array[thread_end].stack = stack;
  for (unsigned i = 0; i < 11; i++) {
    thread_array[thread_end].saved[i] = 0;
  }
  thread_array[thread_end].lr = (uint64_t)func;
  thread_array[thread_end].sp = (uint64_t)stack + PAGE_SIZE;
  thread_array[thread_end].next = 1;
  //
  if (thread_end >= 1) {
    thread_array[thread_end - 1].next = thread_end;
  }
  thread_end += 1;
}

unsigned thread_current() {
  unsigned id = 0;
  asm("mrs %0, tpidr_el1" : "=r"(id));
  return id;
}

void thread_yield() {
  unsigned id = 0;
  uint64_t lr = 0;
  asm("mrs %0, tpidr_el1" : "=r"(id));
  asm("mov %0, x19" : "=r"(thread_array[id].saved[0]));
  asm("mov %0, x20" : "=r"(thread_array[id].saved[1]));
  asm("mov %0, x21" : "=r"(thread_array[id].saved[2]));
  asm("mov %0, x22" : "=r"(thread_array[id].saved[3]));
  asm("mov %0, x23" : "=r"(thread_array[id].saved[4]));
  asm("mov %0, x24" : "=r"(thread_array[id].saved[5]));
  asm("mov %0, x25" : "=r"(thread_array[id].saved[6]));
  asm("mov %0, x26" : "=r"(thread_array[id].saved[7]));
  asm("mov %0, x27" : "=r"(thread_array[id].saved[8]));
  asm("mov %0, x28" : "=r"(thread_array[id].saved[9]));
  asm("mov %0, x29" : "=r"(thread_array[id].saved[10]));
  asm("mov %0, lr" : "=r"(lr));
  asm("ldr %0, =thread_yield_end" : "=r"(thread_array[id].lr));
  asm("mov %0, sp" : "=r"(thread_array[id].sp));
  //
  unsigned next = thread_array[id].next;
  asm("msr tpidr_el1, %0" ::"r"(next));
  asm("mov x19, %0" ::"r"(thread_array[next].saved[0]));
  asm("mov x20, %0" ::"r"(thread_array[next].saved[1]));
  asm("mov x21, %0" ::"r"(thread_array[next].saved[2]));
  asm("mov x22, %0" ::"r"(thread_array[next].saved[3]));
  asm("mov x23, %0" ::"r"(thread_array[next].saved[4]));
  asm("mov x24, %0" ::"r"(thread_array[next].saved[5]));
  asm("mov x25, %0" ::"r"(thread_array[next].saved[6]));
  asm("mov x26, %0" ::"r"(thread_array[next].saved[7]));
  asm("mov x27, %0" ::"r"(thread_array[next].saved[8]));
  asm("mov x28, %0" ::"r"(thread_array[next].saved[9]));
  asm("mov x29, %0" ::"r"(thread_array[next].saved[10]));
  asm("mov lr, %0" ::"r"(thread_array[next].lr));
  asm("mov sp, %0" ::"r"(thread_array[next].sp));
  asm("ret");
  //
  asm("thread_yield_end:");
  asm("mov lr, %0" ::"r"(lr));
}
