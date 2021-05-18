#include "app/app.h"

#include <stddef.h>

#include "page/page.h"
#include "printf/printf.h"

static void* app_return;
static void* app_stack;

int app_execute(void* address) {
  asm("msr spsr_el1, %0" ::"r"(0x340));
  // setup return address
  asm("ldr %0, =app_execute_end" : "=r"(app_return));
  asm("msr elr_el1, %0" ::"r"(address));
  // setup stack memory
  app_stack = page_alloc(0);
  asm("msr sp_el0, %0" ::"r"(app_stack));
  // execute
  asm("eret");
  asm("app_execute_end:");
}

void app_exit() {
  page_free(app_stack);
  // setup for returning
  asm("msr elr_el1, %0" ::"r"(app_return));
  asm("msr spsr_el1, %0" ::"r"(0x3c5));
}
