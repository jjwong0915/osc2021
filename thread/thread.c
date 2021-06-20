#include "thread/thread.h"

#include <stdbool.h>
#include <stdint.h>

#include "page/page.h"
#include "printf/printf.h"

struct thread {
  void* stack;
  bool exited;
  //
  void (*function)(void*);
  void* context;
  //
  uint64_t saved[12];
  uint64_t sp;
  uint64_t pc;
  //
  unsigned prev;
  unsigned next;
};

struct thread thread_array[THREAD_MAX];
static unsigned thread_end;

static void thread_wrapper() {
  unsigned id = thread_current();
  printf("[INFO] thread %d entered\n", id);
  //
  (*thread_array[id].function)(thread_array[id].context);
  //
  thread_exit();
}

void thread_init() {
  // setup main thread
  thread_array[0].prev = 0;
  thread_array[0].next = 0;
  thread_end = 1;
  // set current thread as main thread
  asm("msr tpidr_el1, %0" ::"r"(0));
}

unsigned thread_create(void (*function)(void*), void* context) {
  if (thread_end >= THREAD_MAX) {
    return UINT32_MAX;
  }
  // add thread
  unsigned id = thread_end;
  thread_array[thread_end - 1].next = thread_end;
  thread_end += 1;
  // setup thread
  thread_array[id].stack = page_alloc(0);
  thread_array[id].exited = false;
  thread_array[id].function = function;
  thread_array[id].context = context;
  for (unsigned i = 0; i < 11; i++) {
    thread_array[id].saved[i] = 0;
  }
  thread_array[id].sp = (uint64_t)thread_array[id].stack + PAGE_SIZE;
  thread_array[id].pc = (uint64_t)thread_wrapper;
  thread_array[id].prev = id - 1;
  thread_array[id].next = 0;  // main thread
  //
  return id;
}

unsigned thread_current() {
  unsigned id = 0;
  asm("mrs %0, tpidr_el1" : "=r"(id));
  return id;
}

void thread_yield() {
  unsigned id = 0;
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
  asm("mov %0, x30" : "=r"(thread_array[id].saved[11]));
  asm("mov %0, sp" : "=r"(thread_array[id].sp));
  asm("ldr %0, =thread_yield_end" : "=r"(thread_array[id].pc));
  //
  unsigned next = thread_array[id].next;
  asm("msr tpidr_el1, %0" ::"r"(next));
  asm("mov x16, %0" ::"r"(thread_array[next].pc));
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
  asm("mov x30, %0" ::"r"(thread_array[next].saved[11]));
  asm("mov sp, %0" ::"r"(thread_array[next].sp));
  asm("br x16");
  //
  asm("thread_yield_end:");
}

void thread_exit() {
  unsigned id = thread_current();
  // remove this thread from execution chain
  thread_array[thread_array[id].prev].next = thread_array[id].next;
  thread_array[thread_array[id].next].prev = thread_array[id].prev;
  if (id == thread_end) {
    thread_end -= 1;
  }
  // mark this thread as finished
  thread_array[id].exited = true;
  printf("[INFO] thread %d exited\n", id);
  //
  thread_yield();
}

void thread_clean() {
  for (unsigned i = 0; i < thread_end; i++) {
    if (thread_array[i].exited) {
      page_free(thread_array[i].stack);
      thread_array[i].exited = false;
      printf("[INFO] thread %d cleaned\n", i);
    }
  }
}

unsigned thread_copy(unsigned id) {
  printf("[INFO] thread copy %d\n", id);
  unsigned new_id =
      thread_create(thread_array[id].function, thread_array[id].context);
  // copy resources
  for (unsigned i = 0; i < PAGE_SIZE; i++) {
    *(char*)(thread_array[new_id].stack + i) =
        *(char*)(thread_array[id].stack + i);
  }
  for (unsigned i = 0; i < 12; i++) {
    thread_array[new_id].saved[i] = thread_array[id].saved[i];
  }
  //
  unsigned sp = 0;
  asm("mov %0, sp" : "=r"(sp));
  thread_array[new_id].sp = (uint64_t)thread_array[new_id].stack +
                            (sp - (uint64_t)thread_array[id].stack);
  asm("ldr %0, =thread_copy_end" : "=r"(thread_array[new_id].pc));
  //
  thread_yield();
  asm("thread_copy_end:");
  return new_id;
}
