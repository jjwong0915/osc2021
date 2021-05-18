
struct cpio_file {
  void* address;
  unsigned size;
};

struct cpio_file cpio_open(void* address, char* filename);
