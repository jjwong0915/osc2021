#include <limits.h>

#define VFS_CREATE ((unsigned)0b1)
#define VFS_NOT_FOUND ((unsigned)UINT_MAX)

struct vfs_file;
struct vfs_node {
  void* internal;
  // root node operation
  struct vfs_node* (*lookup)(struct vfs_node* self, char* name);
  struct vfs_node* (*create)(struct vfs_node* self, char* name);
  // file operation
  unsigned (*read)(struct vfs_file* self, unsigned size, void* buffer);
  unsigned (*write)(struct vfs_file* self, unsigned size, void* buffer);
};

struct vfs_backend {
  char* name;
  struct vfs_node* (*setup)(struct vfs_backend* self);
};

struct vfs_file {
  struct vfs_node* node;
  unsigned flags;
  unsigned cursor;
};

void vfs_register(struct vfs_backend* backend);
struct vfs_file* vfs_open(char* pathname, unsigned flags);
unsigned vfs_read(struct vfs_file* file, unsigned size, void* buffer);
unsigned vfs_write(struct vfs_file* file, unsigned size, void* buffer);
