#include "printf/printf.h"
#include "syscall/syscall.h"
#include "vfs/vfs.h"

int main() {
  int a = open("hello", VFS_CREATE);
  int b = open("world", VFS_CREATE);
  write(a, 6, "Hello ");
  write(b, 6, "World!");
  close(a);
  close(b);
  b = open("hello", 0);
  a = open("world", 0);
  char buf[200];
  int sz;
  sz = read(b, 100, buf);
  sz += read(a, 100, buf + sz);
  buf[sz] = '\0';
  printf("%s\n", buf);
  return 0;
}
