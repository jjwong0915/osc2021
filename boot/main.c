#include <stdbool.h>

#include "printf/printf.h"
#include "uart/uart.h"

int value;

int main() {
  uart_init();
  printf("%d\n", 10);
  return 0;
}
