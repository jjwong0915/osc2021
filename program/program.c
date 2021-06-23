#include "program/program.h"

#include <stdbool.h>

#include "page/page.h"
#include "printf/printf.h"

#define INITFS_ADDR ((void*)0x8000000)

void* program_load(char* name) {
  struct cpio_file executable = cpio_open(INITFS_ADDR, name);
  if (executable.size > (PAGE_SIZE << 5)) {
    printf("[ERROR] program executable is bigger than 32 pages");
    while (true) {
      asm("nop");
    }
  }
  //
  void* program_memory = page_alloc(5);
  for (unsigned i = 0; i < executable.size; i++) {
    *(char*)(program_memory + i) = *(char*)(executable.address + i);
  }
  return program_memory;
}

int program_argc(char** argv) {
  int argc = 0;
  if (argv != NULL) {
    while (argv[argc] != NULL) {
      argc += 1;
    }
  }
  return argc;
}
