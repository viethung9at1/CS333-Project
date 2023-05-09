#include "syscall.h"

void *memcpy(void *dest, const void *src, int len) {
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}

int main() {
  char argv[3][100] = {"multi_arg", "Doremon", "Nice"};
  ExecV(3, argv);
  Halt();
}