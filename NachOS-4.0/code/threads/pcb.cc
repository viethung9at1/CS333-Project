#include "pcb.h"

PCB::PCB(int parentID, int pid)
{
  joinsem = new Semaphore("joinsem", 0);
  exitsem = new Semaphore("exitsem", 0);
  multex = new Semaphore("multex", 1);
  exitcode = 0;
  numwait = 0;
  filename = NULL;
  this->parentID = parentID;
  this->pid = pid;
  this->isJoined = false;
}

PCB::~PCB()
{
  delete joinsem;
  delete exitsem;
  delete multex;
  if (filename != NULL)
    delete[] filename;
}

int PCB::Exec(char *filename)
{
  multex->P();
  if (this->filename != NULL)
  {
    multex->V();
    return -1;
  }
  this->filename = new char[strlen(filename) + 1];
  strcpy(this->filename, filename);

  Thread *thread = new Thread(this->filename, this->pid);
  ProcessArg *arg = new ProcessArg(this->pid, 0, NULL);
  thread -> Fork(RunProcess, (void *)arg);
  multex->V();
  return 0;
}

int PCB::Exec(int argc, char **argv)
{
  multex->P();
  if (this->filename != NULL)
  {
    multex->V();
    return -1;
  }
  this->filename = new char[strlen(argv[0]) + 1];
  strcpy(this->filename, argv[0]);
  

  Thread *thread = new Thread(this->filename, this->pid);
  ProcessArg *arg = new ProcessArg(this->pid, argc, argv);
  printf("%d\n", arg);
  thread->Fork(RunProcess, (void *)arg);
  multex->V();
  return 0;
}

void RunProcess(void *arg)
{
  ProcessArg *_arg = (ProcessArg *)arg;
  int pid = _arg->pid;
  int argc = _arg->argc;
  char **argv = _arg->argv;

  char *filename = kernel->pTable->GetFileName(pid);
  kernel->currentThread->space = new AddrSpace;
  AddrSpace *space = kernel->currentThread->space;
  if (!space->Load(filename, argc, argv)){
    return;
  }
  space->Execute();
  ASSERTNOTREACHED();
}

int PCB::GetNumWait() { return numwait; }

void PCB::JoinWait() { joinsem->P(); }

void PCB::ExitWait() { exitsem->P(); }

void PCB::JoinRelease() { joinsem->V(); }

void PCB::ExitRelease() { exitsem->V(); }

void PCB::IncNumWait()
{
  multex->P();
  numwait++;
  multex->V();
}

void PCB::DecNumWait()
{
  multex->P();
  numwait--;
  multex->V();
}

void PCB::SetExitCode(int ec) { exitcode = ec; }

int PCB::GetExitCode() { return exitcode; }

void PCB::SetFileName(char *fn)
{
  if (filename != NULL)
    delete[] filename;
  filename = new char[strlen(fn) + 1];
  strcpy(filename, fn);
}

char *PCB::GetFileName() { return filename; }