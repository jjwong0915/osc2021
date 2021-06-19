#define PAGE_SIZE 0x1000

void page_init();
void* page_alloc(unsigned order);
void page_free(void* address);
