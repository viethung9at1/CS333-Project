#include "syscall.h"
#include "copyright.h" 

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


int main() {
  SpaceId a, b;
  int aa, bb;
  char str[40];
  a = Exec("ping");
  b = Exec("pong");
  //aa = Join(a);
  //bb = Join(b);
  //Write("\n", 1, 1);
  //intToStr(aa, str);
  //Write(str, 32, 1);
  //intToStr(bb, str);
  //Write(str, 32, 1);
  //Exit(0);
}

