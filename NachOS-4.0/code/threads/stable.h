#ifndef STABLE_H
#define STABLE_H

#include "main.h"
#include "sem.h"

class Sem;
#define MAX_PROCESS 10
#define MAX_SEMAPHORE 10
class STable {
private:
  Bitmap *bm; // Manage the free slot
  Sem *semTab[MAX_SEMAPHORE];

public:
  // Initial the Sem object, the started value is null
  // Remember to initial the bm object to use
  STable();
  ~STable();
  int Create(char *name, int init);
  int Wait(char *name);
  int Signal(char *name);
  int FindFreeSlot();
};
#endif