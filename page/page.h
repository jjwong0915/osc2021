#include <stdint.h>

#define PAGE_SIZE ((uint64_t)0x1000)

void page_init();
void* page_alloc(unsigned order);
void page_free(void* address);
