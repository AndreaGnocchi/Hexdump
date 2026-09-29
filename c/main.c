#include "ag.h"
#include "aglib_allocator.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define IS_PRINTABLE_ASCII(c) (((c) > 31) && ((c) < 127))
#define NON_PRINTABLE_ASCII   '.'
#define BUF_SIZE               MB(1)
#define ARENA_SIZE             (BUF_SIZE * 2 + KB(128))

off_t open_read_file(const char* filename, uint8_t* buf, size_t bufCap, sArena* arena) {
  if (!filename || !buf || !arena || !path_exists(filename) || !is_file(filename)) return -1;
  
  int fd = open(filename, O_RDONLY);
  if (fd == -1) return -1;

  sFileBuffer out = {0};
  sAllocator  tmp = use_arena(arena);
  if (!read_all_file(fd, &out, &tmp)) { close(fd); return -1; }
  if (out.size > bufCap) { close(fd); return -1; }
  memcpy(buf, out.data, out.size);

  close(fd);
  return out.size;
}

void hexdump(const void* buf, size_t size) {
  const uint8_t* data = (const uint8_t*)buf;
 
  for (size_t off = 0; off < size; off += 16) {
    size_t n = (size - off < 16) ? size - off : 16;
 
    printf("%08zx  ", off);
 
    for (size_t j = 0; j < 16; ++j) {
      if (j < n) printf("%02x ", data[off + j]);
      else       printf("   ");
      if (j == 7) printf(" ");
    }
 
    printf(" |");
    for (size_t j = 0; j < n; ++j) {
      uint8_t c = data[off + j];
      putchar(IS_PRINTABLE_ASCII(c) ? c : NON_PRINTABLE_ASCII);
    }
    
    printf("|\n");
  }
  
  if (size > 0) printf("%08zx\n", size);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    ERR("Usage: %s <file>", argv[0]);
    return -1;
  }

  sArena arena;
  arena_init(&arena, ARENA_SIZE);

  uint8_t* buf = arena_alloc(&arena, BUF_SIZE, false);
  if (!buf) {
    ERR("Failed to allocate buffer");
    return -1;
  }
  
  off_t openReadResult = open_read_file(argv[1], buf, BUF_SIZE, &arena);

  if (openReadResult == -1) {
    ERR("Failed to open <%s> file", argv[1]);
    return -1;
  }

  size_t size = (size_t)openReadResult;
  hexdump(buf, size);
  
  return 0;
}
