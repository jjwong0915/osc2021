void thread_init();
void thread_create(void (*func)(void));
unsigned thread_current();
void thread_yield();
