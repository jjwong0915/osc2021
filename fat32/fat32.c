#include "fat32/fat32.h"

#include <stdbool.h>

#include "printf/printf.h"
#include "sd/sd.h"
#include "vfs/vfs.h"

#define FAT32_SECTOR_MAX ((unsigned)512)

struct fat32_internal {
  bool is_root;
  unsigned fat_block;
  unsigned data_block;
  unsigned root_index;
  // non-root property
  unsigned this_index;
  unsigned size;
};

struct mbr_partition {
  char status;
  char first_chs[3];
  char type;
  char last_chs[3];
  unsigned first_lba;
  unsigned sector_cnt;
} __attribute__((packed));

struct mbr_structure {
  char bootstrap[446];
  struct mbr_partition partition[4];
} __attribute__((packed));

struct fat32_bpb {
  char dos_bpb[25];
  unsigned sec_cnt;
  char description[2];
  short version;
  unsigned root_index;
  short info_cnt;
} __attribute__((packed));

struct fat32_bootsector {
  char jump_instr[3];
  char oem_name[8];
  struct fat32_bpb bios_param;
} __attribute__((packed));

struct fat32_file {
  char name[8];
  char extension[3];
  char metadata[17];
  unsigned size;
} __attribute__((packed));

static struct vfs_node node_array[FAT32_SECTOR_MAX];
static unsigned node_end;

static struct fat32_internal internal_array[FAT32_SECTOR_MAX];

static struct vfs_node* setup();
static struct vfs_backend backend = {
    .name = "fat32",
    .setup = &setup,
};

static unsigned read(struct vfs_file* self, unsigned size, void* buffer) {
  struct fat32_internal* internal =
      (struct fat32_internal*)self->node->internal;
  unsigned relative_cursor = self->cursor;
  unsigned relative_size = internal->size;
  unsigned current_index = internal->this_index;
  while (relative_cursor > 512) {
    unsigned fat_buffer[128];
    sd_readblock(internal->fat_block + current_index / 128, fat_buffer);
    relative_cursor -= 512;
    relative_size -= 512;
    current_index = fat_buffer[current_index % 128];
  }
  //
  char fat32_buffer[512];
  sd_readblock(internal->data_block + current_index - internal->root_index,
               fat32_buffer);
  unsigned i = 0;
  while (i < size && relative_cursor + i < relative_size &&
         relative_cursor + i < 512) {
    *(char*)(buffer + i) = *(char*)(fat32_buffer + relative_cursor + i);
    i += 1;
  }
  self->cursor += i;
  return i;
}

static unsigned write(struct vfs_file* self, unsigned size, void* buffer) {}

static struct vfs_node* lookup(struct vfs_node* self, char* name) {
  struct fat32_internal* internal = (struct fat32_internal*)(self->internal);
  if (!internal->is_root) {
    printf("[ERROR] fat32 lookup non-root node\n");
    while (true) {
      asm("nop");
    }
  }
  //
  unsigned fat_idx = internal->root_index;
  while (true) {
    unsigned fat_buffer[128];
    sd_readblock(internal->fat_block + fat_idx / 128, fat_buffer);
    struct fat32_file file_list[16];
    sd_readblock(internal->data_block + fat_idx - internal->root_index,
                 file_list);
    // for (unsigned i = 0; i < 16; i++) {
    //   for (unsigned j = 0; j < 21; j++) {
    //     printf("%2x ", file_list[i].metadata[j]);
    //   }
    //   printf("\n");
    // }
    //
    unsigned file_idx = 0;
    while (file_list[file_idx].name[0] != '\0' && file_idx < 16) {
      //
      bool name_correct = true;
      unsigned i = 0;
      while (file_list[file_idx].name[i] != ' ' && i < 8) {
        if (name[i] != file_list[file_idx].name[i]) {
          name_correct = false;
          i += 1;
          break;
        }
        i += 1;
      }
      if (name[i] != '.') {
        name_correct = false;
      }
      i += 1;
      unsigned j = 0;
      while (file_list[file_idx].metadata[j] != ' ' && j < 3) {
        if (name[i] != file_list[file_idx].extension[j]) {
          name_correct = false;
          break;
        }
        i += 1;
        j += 1;
      }
      //
      if (name_correct) {
        if (node_end >= FAT32_SECTOR_MAX) {
          printf("[ERROR] fat32 node maximum exceeded\n");
          while (true) {
            asm("nop");
          }
        }
        unsigned node_idx = node_end;
        node_end += 1;
        //
        struct fat32_internal* new_internal = internal_array + node_idx;
        new_internal->is_root = false;
        new_internal->fat_block = internal->fat_block;
        new_internal->data_block = internal->data_block;
        new_internal->root_index = internal->root_index;
        new_internal->this_index =
            ((unsigned)(*(short*)(file_list[file_idx].metadata + 9)) << 16) +
            (*(short*)(file_list[file_idx].metadata + 15));
        new_internal->size = file_list[file_idx].size;
        printf("[INFO] fat32 file index: %u\n", new_internal->this_index);
        //
        node_array[node_idx].internal = new_internal;
        node_array[node_idx].read = &read;
        node_array[node_idx].write = &write;
        return node_array + node_idx;
      }
      file_idx += 1;
    }
    if (fat_buffer[fat_idx] == 0xffffff0f) {
      break;
    }
    fat_idx = fat_buffer[fat_idx % 128];
  }
  //
  printf("[INFO] fat32 file \"%s\" not found\n", name);
}

static struct vfs_node* create(struct vfs_node* self, char* name) {
  printf("[ERROR] fat32 create operation not implemented\n");
  while (true) {
    asm("nop");
  }
  return NULL;
}

static struct vfs_node* setup() {
  if (node_end >= FAT32_SECTOR_MAX) {
    printf("[ERROR] fat32 node maximum exceeded\n");
    while (true) {
      asm("nop");
    }
  }
  unsigned idx = node_end;
  node_end += 1;
  //
  char mbr_buffer[512];
  sd_readblock(0, mbr_buffer);
  struct mbr_structure* mbr = (struct mbr_structure*)mbr_buffer;
  printf("[INFO] fat32 boot block: %u\n", mbr->partition->first_lba);
  //
  char boot_buffer[512];
  sd_readblock(mbr->partition->first_lba, boot_buffer);
  struct fat32_bootsector* boot_sector = (struct fat32_bootsector*)boot_buffer;
  //
  struct fat32_internal* internal = internal_array + idx;
  internal->is_root = true;
  internal->fat_block = mbr->partition->first_lba + 32;
  internal->data_block = internal->fat_block +
                         boot_sector->bios_param.sec_cnt *
                             *(unsigned*)(boot_sector->bios_param.dos_bpb + 5);
  internal->root_index = boot_sector->bios_param.root_index;
  printf("[INFO] fat32 fat block: %u\n", internal->fat_block);
  printf("[INFO] fat32 data block: %u\n", internal->data_block);
  printf("[INFO] fat32 root index: %u\n", internal->root_index);
  //
  node_array[idx].internal = internal;
  node_array[idx].lookup = &lookup;
  node_array[idx].create = &create;
  //
  return node_array + idx;
}

void fat32_init() {
  printf("[INFO] fat32 initialize sd\n");
  sd_init();
  vfs_register(&backend);
}
