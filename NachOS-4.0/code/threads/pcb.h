#ifndef PCB_H
#define PCB_H

#include "ProcessArg.h"
#include "main.h";
#include "synch.h"

class Semaphore;
// Process Control Block is a data structure that contains information of the
// process related to it
class PCB {
private:
  Semaphore *joinsem; // semaphore for join process
  Semaphore *exitsem; // semaphore for exit process
  Semaphore *multex;  // exclusive access semaphore
  int exitcode;
  int numwait;    // the number of join process
  char *filename; // the name of the process

public:
  bool isJoined;
  int parentID;               // The parent process’s ID
  int pid;                    // The process’s ID
  PCB(int parentID, int pid); // Constructor
  ~PCB();                     // Destructor
  int Exec(char *name); // Create a thread with the name is filename and the
                        // process id is pid
  int Exec(int argc, char **argv);

  int GetNumWait();   // Return the number of the waiting process
  void JoinWait();    // The parent process wait for the child process finishes
  void ExitWait();    // The child process finishes
  void JoinRelease(); // The child process notice the parent process
  void ExitRelease(); // The parent process accept to exit the child process
  void IncNumWait();  // Increase the number of the waiting process
  void DecNumWait();  // Decrease the number of the waiting process
  void SetExitCode(int);    // Set the exit code for the process
  int GetExitCode();        // Return the exitcode
  void SetFileName(char *); // Set the process name
  char *GetFileName();      // Return the process name
};

void RunProcess(void *arg);

#endif

/*

PTable -> tạo ra một cái PCB mới, lúc tạo ra, có parentID và processID

PCB vừa tạo -> Exec("name",


*/