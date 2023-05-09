#include "stable.h"

STable::STable() {
  bm = new Bitmap(MAX_PROCESS);
  for (int i = 0; i < MAX_PROCESS; i++) {
    semTab[i] = NULL;
  }
}

STable::~STable() {
  delete bm;
  for (int i = 0; i < MAX_PROCESS; i++) {
    if (semTab[i] != NULL)
      delete semTab[i];
  }
}

int STable::Create(char *name, int init) {
  int id = FindFreeSlot();
  if (id == -1) {
    return -1;
  }
  semTab[id] = new Sem(name, init);
  return id;
}

int STable::Wait(char *name) {
  for (int i = 0; i < MAX_PROCESS; i++) {
    if (semTab[i] != NULL && strcmp(semTab[i]->GetName(), name) == 0) {
      semTab[i]->wait();
      return 0;
    }
  }
  return -1;
}

int STable::Signal(char *name) {
  for (int i = 0; i < MAX_PROCESS; i++) {
    if (semTab[i] != NULL && strcmp(semTab[i]->GetName(), name) == 0) {
      semTab[i]->signal();
      return 0;
    }
  }
  return -1;
}

int STable::FindFreeSlot() { return bm->FindAndSet(); }