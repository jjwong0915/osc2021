#include "printf/printf.h"
#include "syscall/syscall.h"

int main(int argc, char **argv) {
  printf("Argv Test, pid %u\n", getpid());
  for (int i = 0; i < argc; ++i) {
    printf(argv[i]);
    printf("\n");
  }
  char *fork_argv[] = {"fork_test", 0};
  exec("fork_test", fork_argv);
  return 0;
}
