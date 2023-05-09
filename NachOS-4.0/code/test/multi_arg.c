#include "syscall.h"

void IntToStr(int num, char *str) {
  int i = 0;
  int j = 0;
  char tmp[100];
  if (num == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }
  while (num > 0) {
    tmp[i++] = num % 10 + '0';
    num /= 10;
  }
  tmp[i] = '\0';
  for (j = 0; j < i; j++) {
    str[j] = tmp[i - j - 1];
  }
  str[i] = '\0';
}

int main(int argc, char **argv) {
  char str[100];
  IntToStr(argc, str);
  Write(str, 100, _ConsoleOutput);
  Write(" arguments are: ", 100, _ConsoleOutput);
  Write(argv[0], 100, _ConsoleOutput);
  Write("\n", 100, _ConsoleOutput);
  Write(argv[1], 100, _ConsoleOutput);
  Write("\n", 100, _ConsoleOutput);
  Write(argv[2], 100, _ConsoleOutput);
  Write("\n", 100, _ConsoleOutput);
  return 0;
}