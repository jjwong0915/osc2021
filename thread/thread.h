#define THREAD_MAX (65536)

void thread_init();
unsigned thread_create(void (*function)(void*), void* context);
unsigned thread_current();
void thread_yield();
void thread_exit();
void thread_clean();
unsigned thread_copy(unsigned id);
