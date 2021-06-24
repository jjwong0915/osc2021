#include "tmpfs/tmpfs.h"

#include <stdbool.h>
#include <stddef.h>

#include "cpio/cpio.h"
#include "page/page.h"
#include "printf/printf.h"
#include "vfs/vfs.h"

#define TMPFS_CHILD_MAX ((unsigned)10)
#define TMPFS_NAME_MAX ((unsigned)20)
#define TMPFS_NODE_MAX ((unsigned)100)
#define INITFS_ADDR ((void*)0x8000000)

struct tmpfs_internal {
  bool is_root;
  // root property
  struct vfs_node* child_array[TMPFS_CHILD_MAX];
  unsigned child_end;
  // file property
  char name[TMPFS_NAME_MAX];
  unsigned size;
  void* data;
};

static struct vfs_node* setup();
static struct vfs_backend backend = {
    .name = "tmpfs",
    .setup = &setup,
};

static struct vfs_node node_array[TMPFS_NODE_MAX];
static unsigned node_end;
static struct tmpfs_internal internal_array[TMPFS_NODE_MAX];

static bool string_equal(const char* s1, const char* s2) {
  unsigned idx = 0;
  while (s1[idx] != '\0' && s2[idx] != '\0') {
    if (s1[idx] != s2[idx]) {
      break;
    }
    idx += 1;
  }
  if (s1[idx] == '\0' && s2[idx] == '\0') {
    return true;
  }
  return false;
}

static unsigned read(struct vfs_file* self, unsigned size, void* buffer) {
  struct tmpfs_internal* internal =
      (struct tmpfs_internal*)(self->node->internal);
  printf("[INFO] tmpfs read \"%s\" size %u cursor %u total_size %u\n",
         internal->name, size, self->cursor, internal->size);
  unsigned i = 0;
  while (i < size && self->cursor + i < internal->size) {
    *(char*)(buffer + i) = *(char*)(internal->data + self->cursor + i);
    i += 1;
  }
  self->cursor += i;
  return i;
}

static unsigned write(struct vfs_file* self, unsigned size, void* buffer) {
  struct tmpfs_internal* internal =
      (struct tmpfs_internal*)(self->node->internal);
  unsigned i = 0;
  while (i < size && self->cursor + i < (PAGE_SIZE << 5)) {
    *(char*)(internal->data + self->cursor + i) = *(char*)(buffer + i);
    i += 1;
  }
  self->cursor += i;
  if (self->cursor > internal->size) {
    internal->size = self->cursor;
  }
  return i;
}

static struct vfs_node* lookup(struct vfs_node* self, char* name) {
  printf("[INFO] tmpfs lookup \"%s\"\n", name);
  struct tmpfs_internal* internal = (struct tmpfs_internal*)(self->internal);
  if (!internal->is_root) {
    printf("[ERROR] tmpfs lookup non-root node\n");
    while (true) {
      asm("nop");
    }
  }
  //
  for (unsigned i = 0; i < internal->child_end; i++) {
    struct vfs_node* child = internal->child_array[i];
    struct tmpfs_internal* child_internal =
        (struct tmpfs_internal*)(child->internal);
    printf("[INFO] tmpfs child %u: %s\n", i, child_internal->name);
    if (string_equal(child_internal->name, name)) {
      return child;
    }
  }
  return NULL;
}

static struct vfs_node* create(struct vfs_node* self, char* name) {
  printf("[INFO] tmpfs create \"%s\"\n", name);
  if (node_end >= TMPFS_NODE_MAX) {
    printf("[ERROR] tmpfs node maximum exceeded\n");
    while (true) {
      asm("nop");
    }
  }
  unsigned node_idx = node_end;
  node_end += 1;
  // setup internal
  struct tmpfs_internal* child_internal = internal_array + node_idx;
  child_internal->is_root = false;
  for (unsigned i = 0; i < TMPFS_NAME_MAX; i++) {
    child_internal->name[i] = name[i];
  }
  child_internal->size = 0;
  child_internal->data = page_alloc(5);
  // setup node
  struct vfs_node* node = node_array + node_idx;
  node->internal = child_internal;
  node->read = &read;
  node->write = &write;
  // add as child
  struct tmpfs_internal* self_internal = (struct tmpfs_internal*)self->internal;
  if (self_internal->child_end >= TMPFS_CHILD_MAX) {
    printf("[ERROR] tmpfs child maximum exceeded\n");
    while (true) {
      asm("nop");
    }
  }
  unsigned child_idx = self_internal->child_end;
  self_internal->child_end += 1;
  self_internal->child_array[child_idx] = node;
  //
  return node;
}

struct vfs_node* setup() {
  if (node_end >= TMPFS_NODE_MAX) {
    printf("[ERROR] tmpfs node maximum exceeded\n");
    while (true) {
      asm("nop");
    }
  }
  unsigned idx = node_end;
  node_end += 1;
  //
  struct tmpfs_internal* internal = internal_array + idx;
  internal->is_root = true;
  internal->child_end = 0;
  node_array[idx].internal = internal;
  node_array[idx].lookup = &lookup;
  node_array[idx].create = &create;
  //
  return node_array + idx;
}

void tmpfs_init() {
  vfs_register(&backend);
  printf("[INFO] tmpfs get initfs file list\n");
  char filename_array[CPIO_FILE_MAX][CPIO_NAME_MAX];
  cpio_list(INITFS_ADDR, filename_array);
  unsigned idx = 0;
  while (filename_array[idx][0] != '\0') {
    struct cpio_file cfile = cpio_open(INITFS_ADDR, filename_array[idx]);
    struct vfs_file* vfile = vfs_open(filename_array[idx], VFS_CREATE);
    vfs_write(vfile, cfile.size, cfile.address);
    idx += 1;
  }
}
