#include "printf/printf.h"

int main() __attribute__((section(".text.startup")));

void hello() {
  printf("Hello ");
  printf("World!\n");
}

int main() {
  hello();
  return 0;
}
