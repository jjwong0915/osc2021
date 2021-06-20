#include "cpio/cpio.h"

#include <stdbool.h>
#include <stdlib.h>

#include "printf/printf.h"

struct header {
  char magic[6];
  char ino[8];
  char mode[8];
  char uid[8];
  char gid[8];
  char nlink[8];
  char mtime[8];
  char filesize[8];
  char devmajor[8];
  char devminor[8];
  char rdevmajor[8];
  char rdevminor[8];
  char namesize[8];
  char check[8];
};

static bool streq(char* left, char* right, unsigned length) {
  for (unsigned i = 0; i < length; i++) {
    if (left[i] != right[i]) {
      return false;
    }
  }
  return true;
}

static unsigned hex2uint(char* str) {
  unsigned num = 0;
  for (unsigned i = 0; i < 8; i++) {
    num *= 16;
    if (str[i] >= '0' && str[i] <= '9') {
      num += str[i] - '0';
    } else if (str[i] >= 'A' && str[i] <= 'F') {
      num += str[i] - 'A' + 10;
    }
  }
  return num;
}

struct cpio_file cpio_open(void* address, char* filename) {
  struct cpio_file file = {.address = NULL, .size = 0};
  while (true) {
    if (!streq(address, "070701", 6)) {
      break;
    }
    struct header* stat = (struct header*)address;
    void* name = address + sizeof(struct header);
    unsigned namesize = hex2uint(stat->namesize);
    void* data = name + namesize + (4 - (namesize + 2) % 4) % 4;
    if (streq(name, filename, namesize)) {
      file.address = data;
      file.size = hex2uint(stat->filesize);
      break;
    }
    address = data + hex2uint(stat->filesize);
  }
  if (file.address == NULL) {
    printf("[ERROR] file \"%s\" not found\n", filename);
    // pause execution
    while (true) {
      asm("nop");
    }
  }
  return file;
}
