#include "vfs/vfs.h"

#include <stdbool.h>

#include "printf/printf.h"

#define VFS_BACKEND_MAX ((unsigned)10)
#define VFS_FILE_MAX ((unsigned)100)

static struct vfs_node* root;
static struct vfs_backend* backend_array[VFS_BACKEND_MAX];
static unsigned backend_end;
static struct vfs_file file_array[VFS_FILE_MAX];
unsigned file_end;

void vfs_register(struct vfs_backend* backend) {
  printf("[INFO] vfs register \"%s\"\n", backend->name);
  if (backend_end >= VFS_BACKEND_MAX) {
    printf("[ERROR] vfs maximum backend number exceeded");
    while (true) {
      asm("nop");
    }
  }
  unsigned idx = backend_end;
  backend_array[idx] = backend;
  backend_end += 1;
  //
  if (root == NULL) {
    root = backend->setup();
  }
}

struct vfs_file* vfs_open(char* pathname, unsigned flags) {
  printf("[INFO] vfs open \"%s\"\n", pathname);
  if (file_end >= VFS_FILE_MAX) {
    printf("[ERROR] vfs file maximum exceeded\n");
    while (true) {
      asm("nop");
    }
  }
  unsigned idx = file_end;
  file_end += 1;
  // setup node
  struct vfs_node* node = root->lookup(root, pathname);
  if (node == NULL && flags & VFS_CREATE) {
    node = root->create(root, pathname);
  }
  // setup file
  struct vfs_file* file = file_array + idx;
  file->node = node;
  file->flags = flags;
  file->cursor = 0;
  return file;
}

unsigned vfs_read(struct vfs_file* file, unsigned size, void* buffer) {
  return file->node->read(file, size, buffer);
}

unsigned vfs_write(struct vfs_file* file, unsigned size, void* buffer) {
  return file->node->write(file, size, buffer);
}
