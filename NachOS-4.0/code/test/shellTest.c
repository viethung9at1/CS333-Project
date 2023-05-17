#include "syscall.h"

#define BUFFER_SIZE 100

int argc = 0;
char argv[5][BUFFER_SIZE];

int _strcpy(char *dest, const char *src) {
  int i = 0;
  while (src[i] != '\0') {
    dest[i] = src[i];
    ++i;
  }
  dest[i] = '\0';
  return i;
}

int _strcmp(char *s1, char *s2) {
  int i = 0;
  while (s1[i] != '\0' && s2[i] != '\0') {
    if (s1[i] != s2[i])
      if (s1[i] < s2[i])
        return -1;
      else
        return 1;
    ++i;
  }
  if (s1[i] == '\0' && s2[i] == '\0')
    return 0;
  if (s1[i] == '\0')
    return -1;
  return 1;
}

void print(char *str) {
  int len;
  char msg[BUFFER_SIZE];

  len = _strcpy(msg, str);
  Write(msg, len, 1);
}


void intToStr(int num, char *str) {
    
    int len = 0;
    int tmp = 0;
    int i;

    if (num == 0) {
        *str++ = '0';
        *str = '\0';
        return;
    }

    if (num < 0) {
        *str++ = '-';
        num = -num;
    }

    tmp = num;

    while (tmp) {
        len++;
        tmp /= 10;
    }

    str[len] = '\0';

    for ( i = len - 1; i >= 0; i--) {
        str[i] = num % 10 + '0';
        num /= 10;
    }
}


void split(char *str) {
  int i = 0, j = 0, k = 0;
  while (str[i] != '\0') {
    if (str[i] == ' ') {
      if (k > 0) {
        argv[j][k] = '\0';
        ++j;
        k = 0;
      }
    } else {
      argv[j][k] = str[i];
      ++k;
    }
    ++i;
  }
  if (k > 0) {
    argv[j][k] = '\0';
    ++j;
  }
  argc = j;
}

int main()
{
    int proc, status;
    char buffer[BUFFER_SIZE], tmp[50];
    SpaceId newProc;

    while(1){
        print("Shell: ");
        Read(buffer, 60, 0);
        split(buffer);

        if (argc == 0)
            continue;
        if (argc == 1 && _strcmp(argv[0], "exit") == 0)
            break;
        proc = Exec(argv[0]);

        if (newProc == -1) {
            print("Error, executable not found.\n");
        } else {
            status = Join(newProc);
            print("OK");
            //intToStr(status, tmp);
            //print(tmp);
        }
    }
    Exit(0);
}