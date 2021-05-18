#include "printf/printf.h"
#include "uart/uart.h"

int main() {
  char hello[] = "Hello World!\n";
  asm("svc 15");
  printf(hello);
  for (int i = 0; i < 1000000000; i++) {
    asm("nop");
  }
  return 2;
}
