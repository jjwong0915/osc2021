
#define CPIO_NAME_MAX ((unsigned)20)
#define CPIO_FILE_MAX ((unsigned)100)

struct cpio_file {
  void* address;
  unsigned size;
};

struct cpio_file cpio_open(void* address, char* filename);
void cpio_list(void* address, char buffer[CPIO_FILE_MAX][CPIO_NAME_MAX]);
