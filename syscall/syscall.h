struct syscall_exec {
  char* name;
  char* const* argv;
  int result;
};

unsigned getpid();
void uart_send(unsigned int c);
char uart_getc();
int exec(char* name, char* const argv[]);
void exit();
unsigned fork();
