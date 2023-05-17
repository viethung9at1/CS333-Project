#include "syscall.h"

int main() {
  //   Halt();
  int sem = CreateSemaphore("lock", 1);
  int id1 = Exec("testSem");
  int id2 = Exec("testSem");
  int id3 = Exec("testSem");
  Join(id1);
  Join(id2);
  Join(id3);
  Exit(0);
}