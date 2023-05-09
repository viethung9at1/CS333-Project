#include "syscall.h"

int main() {
  SpaceId a;
  SpaceId b;

  a = Exec("ping");
  b = Exec("pong");

  Join(a);
  Join(b);

}

