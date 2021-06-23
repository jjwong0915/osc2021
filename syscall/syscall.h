struct syscall_exec {
  char* name;
  char** argv;
  int result;
};

struct syscall_open {
  char* pathname;
  unsigned flags;
  unsigned result;
};

struct syscall_read {
  unsigned fd;
  unsigned size;
  void* buffer;
  unsigned result;
};

struct syscall_write {
  unsigned fd;
  unsigned size;
  void* buffer;
  unsigned result;
};

unsigned getpid();
void uart_send(unsigned int c);
char uart_getc();
int exec(char* name, char** argv);
void exit();
unsigned fork();
unsigned open(char* pathname, unsigned flags);
void close(unsigned fd);
unsigned read(unsigned fd, unsigned size, void* buffer);
unsigned write(unsigned fd, unsigned size, void* buffer);
