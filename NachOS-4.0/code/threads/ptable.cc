#include "ptable.h"
#include "main.h"
PTable::PTable(int size) {
  psize = size;
  bm = new Bitmap(size);
  bmsem = new Semaphore("bmsem", 1);
  for (int i = 0; i < size; i++) {
    pcb[i] = NULL;
  }
  pcb[0] = new PCB(0, 0);
  bm->Mark(0);
}

PTable::~PTable() {
  delete bm;
  delete bmsem;
  for (int i = 0; i < psize; i++) {
    if (pcb[i] != NULL)
      delete pcb[i];
  }
}

int PTable::ExecUpdate(char *name) {
  bmsem->P();
  int pid = bm->FindAndSet();
  if (pid == -1) {
    bmsem->V();
    return -1;
  }
  pcb[pid] = new PCB(kernel->currentThread->processID, pid);
  bmsem->V();
  pcb[pid]->Exec(name);
  return pid;
}

int PTable::ExecUpdate(int argc, char **argv) {
  bmsem->P();
  int pid = bm->FindAndSet();
  if (pid == -1) {
    bmsem->V();
    return -1;
  }
  pcb[pid] = new PCB(kernel->currentThread->processID, pid);
  bmsem->V();
  pcb[pid]->Exec(argc, argv);
  return pid;
}

int PTable::JoinUpdate(int pid) {
  int parentID = kernel->currentThread->processID;
  int childID = pid;
  if (pcb[pid] == NULL || pcb[pid]->parentID != parentID) {
    return -1;
  }
  // pcb[childID]->isJoined = true;
  pcb[childID]->JoinWait();
  pcb[parentID]->IncNumWait();
  pcb[parentID]->ExitRelease();
  return 0;
} // Process the syscall SC_Join

int PTable::ExitUpdate(int ec) {
  int pid = kernel->currentThread->processID;
  if (pid == 0) {
    return -1;
  }
  pcb[pid]->SetExitCode(ec);
  // if (pcb[pid]->isJoined == true) {
  int parentID = pcb[pid]->parentID;
  pcb[pid]->JoinRelease();
  pcb[parentID]->DecNumWait();
  pcb[parentID]->ExitWait();
  // }
  // printf("[PTable/ExitUpdate] %d\n", pid);
  // delete pcb[pid];
  // pcb[pid] = NULL;
  // bm->Clear(pid);
  return 0;
}

char *PTable::GetFileName(int id) {
  if (pcb[id] == NULL) {
    return NULL;
  }
  return pcb[id]->GetFileName();
}