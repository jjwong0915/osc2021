#include "process/process.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>

#include "page/page.h"
#include "printf/printf.h"
#include "program/program.h"
#include "thread/thread.h"

struct process {
  void* program;
  char** argv;
  //
  unsigned thread;
};

struct process process_array[THREAD_MAX];
unsigned process_end;

static void process_wrapper() {
  unsigned id = process_current();
  printf("[INFO] process %d entered\n", id);
  //
  asm("mov x16, %0" ::"r"(program_argc(process_array[id].argv)));
  asm("mov x17, %0" ::"r"(process_array + id));
  asm("mov x0, x16");
  asm("ldp x2, x1, [x17]");
  asm("blr x2");
  //
  process_exit();
}

void process_init() { process_end = 1; }

void process_run(char* filename, char** argv) {
  unsigned id = process_end;
  process_end += 1;
  //
  process_array[id].program = program_load(filename);
  process_array[id].argv = argv;
  process_array[id].thread = thread_create(process_wrapper, NULL);
  thread_yield();
}

unsigned process_current() {
  unsigned thread = thread_current();
  for (unsigned i = 0; i < process_end; i++) {
    if (process_array[i].thread == thread) {
      return i;
    }
  }
  return UINT_MAX;
}

void process_exit() {
  unsigned id = process_current();
  printf("[INFO] process %d exited\n", id);
  thread_exit();
}

unsigned process_fork() {
  unsigned id = process_current();
  unsigned child_id = process_end;
  process_end += 1;
  //
  process_array[child_id].program = process_array[id].program;
  process_array[child_id].argv = process_array[id].argv;
  process_array[child_id].thread = thread_copy(process_array[id].thread);
  //
  if (process_current() == child_id) {
    return 0;
  }
  return child_id;
}
