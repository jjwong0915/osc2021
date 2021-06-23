void process_init();
void process_run(char* file, char** argv);
unsigned process_current();
void process_exit();
unsigned process_fork();
unsigned process_open(char* pathname, unsigned flags);
void process_close(unsigned fd);
unsigned process_read(unsigned fd, unsigned size, void* buffer);
unsigned process_write(unsigned fd, unsigned size, void* buffer);
