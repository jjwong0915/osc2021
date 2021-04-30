#include "cpio.h"

#include <stdbool.h>
#include <stdlib.h>

bool cpio_memcmp(char* left, char* right) {
  for (int i = 0; i < 6; i++) {
    if (left[i] != right[i]) {
      return true;
    }
  }
  return false;
}

int cpio_hex2dec(char* str) {
  int num = 0;
  for (int i = 0; i < 8; i++) {
    num *= 16;
    if (str[i] >= '0' && str[i] <= '9') {
      num += str[i] - '0';
    } else if (str[i] >= 'A' && str[i] <= 'F') {
      num += str[i] - 'A' + 10;
    }
  }
  return num;
}

void* cpio_open(void* addr, char* filename) {
  while (true) {
    if (!cpio_memcmp(addr, "070701")) {
      break;
    }
    // cpio_header* header = (cpio_header*)addr;
  }
  return NULL;
}
