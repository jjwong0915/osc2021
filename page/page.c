#include "page/page.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>

#include "printf/printf.h"

#define PAGE_MAX 12
#define PAGE_SIZE 0x1000
#define PAGE_BEGIN 0x10000000
#define PAGE_END 0x40000000
#define PAGE_CNT ((PAGE_END - PAGE_BEGIN) / PAGE_SIZE)

struct page {
  unsigned order;
  bool allocated;
  // used for freelist
  struct page* prev;
  struct page* next;
};

static struct page page_array[PAGE_CNT];
static struct page page_freelist[PAGE_MAX + 1];

static void freelist_erase(struct page* target_page) {
  target_page->prev->next = target_page->next;
  target_page->next->prev = target_page->prev;
}

static void freelist_push(struct page* new_page) {
  struct page* target_page = page_freelist[new_page->order].next;
  if (page_freelist[new_page->order].next == NULL) {
    new_page->prev = page_freelist + new_page->order;
    new_page->next = NULL;
    page_freelist[new_page->order].next = new_page;
  } else {
    new_page->prev = target_page->prev;
    new_page->next = target_page;
    target_page->prev->next = new_page;
    target_page->prev = new_page;
  }
}

static struct page* freelist_pop(unsigned order) {
  struct page* free_page = page_freelist[order].next;
  freelist_erase(free_page);
  return free_page;
}

void page_init() {
  unsigned index = 0;
  while (index < PAGE_CNT) {
    struct page* free_page = page_array + index;
    free_page->order = PAGE_MAX;
    freelist_push(free_page);
    index += (1 << PAGE_MAX);
  }
}

void* page_alloc(unsigned order) {
  // find fittable free block
  unsigned free_order = order;
  while (true) {
    if (page_freelist[free_order].next != NULL) {
      break;
    }
    free_order += 1;
    if (free_order > PAGE_MAX) {
      return NULL;
    }
  }
  // find page
  struct page* free_page = freelist_pop(free_order);
  free_page->allocated = true;
  while (true) {
    if (free_page->order == order) {
      break;
    } else if (free_page->order < order) {
      // should not happen
      return NULL;
    }
    // split big page
    free_page->order -= 1;
    struct page* remain_page = free_page + (1 << free_page->order);
    remain_page->order = free_page->order;
    freelist_push(remain_page);
  }
  return (void*)(PAGE_BEGIN + PAGE_SIZE * (free_page - page_array));
}

void page_free(void* address) {
  unsigned free_index = ((unsigned long long)address - PAGE_BEGIN) / PAGE_SIZE;
  if (free_index > PAGE_CNT || !page_array[free_index].allocated) {
    return;
  }
  // merge free block
  page_array[free_index].allocated = false;
  while (page_array[free_index].order < PAGE_MAX) {
    unsigned buddy_index = free_index ^ (1 << page_array[free_index].order);
    if (page_array[buddy_index].allocated ||
        page_array[buddy_index].order != page_array[free_index].order) {
      break;
    }
    freelist_erase(page_array + buddy_index);
    free_index &= ~(1 << page_array[free_index].order);
    page_array[free_index].order += 1;
  }
  freelist_push(page_array + free_index);
}
