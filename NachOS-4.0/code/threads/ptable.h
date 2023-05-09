#ifndef PTABLE_H
#define PTABLE_H

#include "main.h";
#include "pcb.h"
#include "synch.h";

class PCB;
class Semaphore;

#define MAX_PROCESS 10
class PTable {
private:
  int psize;
  Bitmap *bm;
  PCB *pcb[MAX_PROCESS];
  Semaphore *bmsem;
public:
  PTable(int size);

  ~PTable();              
  int ExecUpdate(char *);
  int ExitUpdate(int);  
  int JoinUpdate(int); 
  int GetFreeSlot(); 
  int ExecUpdate(int, char **);

  bool IsExist(int pid);
  void Remove(int pid);
  char *GetFileName(int id);
};

#endif