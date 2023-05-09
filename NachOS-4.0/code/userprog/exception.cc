// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (case can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "ksyscall.h"
#include "main.h"
#include "syscall.h"
// #include "machine.h"
#include "filesys.h"
#include "kernel.h"
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_LENGTH_IP_ADDRESS 46

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"case" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------


void handle_SC_Add();
void handle_SC_Create();
void handle_SC_Open();
void handle_SC_Close();
void handle_SC_Read();
void handle_SC_Write();
void handle_SC_Seek();
void handle_SC_Remove();
// SOCKET
void handle_SC_SocketTCP_Open();
void handle_SC_SocketTCP_Connect();
//void handle_SC_SocketTCP_Send();
//void handle_SC_SocketTCP_Receive();
//void handle_SC_SocketTCP_Close();


// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to S    case SC_Add:
	
char* User2System(int vAddr,int limit) 
{ 
	int i;// index 
	int char01; 
	char* kernelBuf = NULL; 
	kernelBuf = new char[limit + 1]; // terminal string here
	if (kernelBuf == NULL) 
	return kernelBuf; 

	memset(kernelBuf, 0, limit + 1); 

	for (i = 0 ; i < limit ;i++) { 
		kernel -> machine -> ReadMem(vAddr + i, 1 , &char01); 
		kernelBuf[i] = (char)char01; 
		if (char01 == 0) 
		break; 
	} 
	return kernelBuf; 
}

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int vAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0) return len;
	int i = 0, char01 = 0;

	do{
		char01 = (int)buffer[i];
		kernel -> machine -> WriteMem(vAddr + i, 1, char01);
		i++;
	} while (i < len && char01 != 0);

	return i;
}

/* Modify return point */
void PCIncrease() {
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	kernel->machine->WriteRegister(PCReg, kernel -> machine -> ReadRegister(PCReg) + 4);

	kernel->machine->WriteRegister(NextPCReg, kernel -> machine -> ReadRegister(PCReg) + 4);
}

void sysHalt() {
  DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
  SysHalt();
}

void handle_SC_Add() {
  DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

  int result;
  result = SysAdd( (int)kernel->machine->ReadRegister(4), (int)kernel->machine->ReadRegister(5));

  DEBUG(dbgSys, "Add returning with " << result << "\n");
  kernel->machine->WriteRegister(2, (int)result);
}

void handle_SC_Create() {
  int vAddr;
  char *filename;
  DEBUG(dbgFile, "\n SC_Create call ...");
  DEBUG(dbgFile, "\n Reading virtual address of filename");
  vAddr = kernel->machine->ReadRegister(4);
  filename = User2System(vAddr, MaxFileLength + 1);
  if (filename == NULL) {
    printf("\n Not enough memory in system");
    DEBUG(dbgFile, "\n Not enough memory in system");
    kernel->machine->WriteRegister(
        2, -1); 
    delete[] filename;
    return;
  }

  DEBUG(dbgFile, "\n Finish reading filename.");
  if (!kernel->fileSystem->Create(filename, 0)) {
    printf("\n Error create file '%s'", filename);
    kernel->machine->WriteRegister(2, -1);
    delete[] filename;
    return;
  }
  kernel->machine->WriteRegister(
      2, 0);

  delete[] filename;
}

void handle_SC_Open() {
  int vAddr;
  char *filename;
  DEBUG(dbgFile, "\n SC_Open call ...");
  vAddr = kernel->machine->ReadRegister(4);
  DEBUG(dbgFile, "\n Reading filename.");
  filename = User2System(vAddr, MaxFileLength + 1); 
  if (filename == NULL) {
    printf("\n Not enough memory in system");
    DEBUG(dbgFile, "\n Not enough memory in system");
    kernel->machine->WriteRegister(
        2, -1); 
    delete[] filename;
    return;
  }
  int type = (int)kernel->machine->ReadRegister(5);
  int returnId = -1;
  if (type == 0) {
    OpenFile *file = kernel->fileSystem->Open(filename);
    if (file == NULL) {
      kernel->machine->WriteRegister(2, -1);
      delete[] filename;
      return;
    }
    returnId = file -> getFileDescriptorID();
  } else if (type == 1) {
    OpenFile *file = kernel->fileSystem -> OpenReadOnly(filename);
    if (file == NULL) {
      kernel->machine->WriteRegister(2, -1);
      delete[] filename;
      return;
    }
    returnId = file -> getFileDescriptorID();
  } else {
    kernel->machine->WriteRegister(
        2, -1);
    delete[] filename;
    return;
  }

  kernel->machine->WriteRegister(2, returnId);
  delete[] filename;
}

void handle_SC_Close() {
  int IdFile = (int)kernel->machine->ReadRegister(4);
  int ret = kernel -> fileSystem -> CloseFile(IdFile);
  kernel->machine->WriteRegister(2, ret);
}

void handle_SC_Write() {
  int vAddr = kernel->machine->ReadRegister(4);
  int demChar = kernel->machine->ReadRegister(5);
  char *buffer = User2System(vAddr, demChar);
  int IdFile = kernel->machine->ReadRegister(6);

  DEBUG(dbgFile, "Write is called\n");

  kernel->machine->WriteRegister(2, SysWrite(buffer, demChar, IdFile));

  delete[] buffer;
}

void handle_SC_Read() {
  int vAddr = kernel->machine->ReadRegister(4);
  int demChar = kernel->machine->ReadRegister(5);
  int IdFile = kernel->machine->ReadRegister(6);

  DEBUG(dbgFile, "Read is called\n");
  char *buffer = new char[demChar + 1];
  int actualLen = SysRead(buffer, demChar, IdFile);

  System2User(vAddr, actualLen, buffer);
  kernel->machine->WriteRegister(2, actualLen);
}

void handle_SC_Seek() {
  int pos = kernel->machine->ReadRegister(4);
  int IdFile = kernel->machine->ReadRegister(5);

  kernel->machine->WriteRegister(2, SysSeek(pos, IdFile));
}

void handle_SC_Remove() {
  int vAddr;
  char *filename;
  vAddr = kernel->machine->ReadRegister(4);
  filename = User2System(vAddr, MaxFileLength + 1); 

  if (filename == NULL) {
    printf("\n Not enough memory in system");
    DEBUG(dbgFile, "\n Not enough memory in system");
    kernel->machine->WriteRegister(2, -1);
    delete[] filename;
    return;
  }

  kernel->machine->WriteRegister(2, SysRemove(filename));
}

void handle_SC_SocketTCP_Open() { kernel->machine->WriteRegister(2, SysSocket()); }

void handle_SC_SocketTCP_Connect() {
  int socketId = kernel->machine->ReadRegister(4);
  int vAddr = kernel->machine->ReadRegister(5);
  char *ip = User2System(vAddr, MAX_LENGTH_IP_ADDRESS);
  int port = kernel->machine->ReadRegister(6);

  kernel->machine->WriteRegister(2, SysConnect(socketId, ip, port));

  delete[] ip;
}

void SysExec() {
  int vAddr = kernel->machine->ReadRegister(4);
  char *filename = User2System(vAddr, MaxFileLength + 1);

  if (filename == NULL) {
    printf("\n Not enough memory in system");
    DEBUG(dbgFile, "\n Not enough memory in system");
    kernel->machine->WriteRegister(2, -1); 
    return;
  }
  OpenFile *executable = kernel -> fileSystem -> Open(filename);
  if (executable == NULL) {
    printf("Cannot open file\n");
    kernel->machine->WriteRegister(2, -1);
    delete[] filename;
    return;
  }

  int id = kernel -> pTable -> ExecUpdate(filename);
  kernel -> machine -> WriteRegister(2, id);
  delete[] filename;
}

void SysJoin() {
  int id = kernel -> machine -> ReadRegister(4);
  kernel -> machine -> WriteRegister(2, kernel -> pTable -> JoinUpdate(id));
}

void SysExit() {
  int a = kernel -> machine -> ReadRegister(4);
  kernel->machine->WriteRegister(2, kernel -> pTable -> ExitUpdate(a));

  if (kernel -> currentThread -> pId == 0) SysHalt(); 
  else kernel -> currentThread -> Finish();
}

void sysCreateSemaphore() {
  int vAddr = kernel -> machine -> ReadRegister(4);
  char *name = User2System(vAddr, MaxFileLength + 1);

  if (name == NULL) {
    printf("\n Not enough memory in system");
    DEBUG(dbgFile, "\n Not enough memory in system");
    kernel -> machine -> WriteRegister(2, -1);
    return;
  }

  int sVal = kernel -> machine -> ReadRegister(5);
  kernel -> machine -> WriteRegister(2, kernel -> sTable -> Create(name, sVal));
}

void sysSignal() {
  int vAddr = kernel -> machine -> ReadRegister(4);

  char *name = User2System(vAddr, MaxFileLength + 1);
  if (name == NULL) {
    printf("\n Not enough memory in system");
    DEBUG(dbgFile, "\n Not enough memory in system");
    kernel -> machine -> WriteRegister(2, -1);
    return;
  }

  kernel -> machine -> WriteRegister(2, kernel -> sTable -> Signal(name));
}

void sysWait() {
  int vAddr = kernel -> machine -> ReadRegister(4);
  char *name = User2System(vAddr, MaxFileLength + 1);

  if (name == NULL) {
    printf("\n Not enough memory in system");
    DEBUG(dbgFile, "\n Not enough memory in system");
    kernel -> machine -> WriteRegister(2, -1); 
    return;
  }

  int n = kernel -> sTable -> Wait(name);
  kernel -> machine -> WriteRegister(2, n); 
}


void SysExecV() {
  int argc = kernel -> machine -> ReadRegister(4);
  int vAddr = kernel -> machine -> ReadRegister(5);
  
  char **argv = new char *[argc];
  
  for (int i = 0; i < argc; i++) {
    argv[i] = User2System(vAddr, MaxFileLength + 1);
    vAddr += 100;
  }

  kernel -> machine -> WriteRegister(2, kernel -> pTable -> ExecUpdate(argc, argv));
  for (int i = 0; i < argc; i++) {
    delete[] argv[i];
  }
  delete[] argv;
}

void ExceptionHandler(ExceptionType which) {
  int type = kernel -> machine -> ReadRegister(2);

  DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

  switch (which) {
  case SyscallException:
    switch (type) {
    case SC_Halt:
      sysHalt();
      ASSERTNOTREACHED();
      break;

    case SC_Add:
      handle_SC_Add();
      return PCIncrease();

    case SC_Create:
      handle_SC_Create();
      return PCIncrease();

    case SC_Open:
      handle_SC_Open();
      handle_SC_Create();
      return PCIncrease();

    case SC_Close:
      handle_SC_Close();
      return PCIncrease();

    case SC_Write:
      handle_SC_Write();
      return PCIncrease();
    case SC_Read:
      handle_SC_Read();
      return PCIncrease();
    case SC_Seek:
      handle_SC_Seek();
      return PCIncrease();

    case SC_Remove:
      handle_SC_Remove();
      return PCIncrease();

    case SC_SocketTCP_Open:
      handle_SC_SocketTCP_Open();
      return PCIncrease();

    case SC_SocketTCP_Connect:
      handle_SC_SocketTCP_Connect();
      return PCIncrease();

    case SC_Exec:
      SysExec();
      return PCIncrease();

    case SC_Join:
      SysJoin();
      return PCIncrease();

    case SC_Exit:
      SysExit();
      return PCIncrease();

    case SC_CreateSemaphore:
      sysCreateSemaphore();
      return PCIncrease();

    case SC_Signal:
      sysSignal();
      return PCIncrease();

    case SC_Wait:
      sysWait();
      return PCIncrease();

    case SC_ExecV:
      SysExecV();
      return PCIncrease();

    default:
      cerr << "Unexpected system call " << type << "\n";
      break;
    }
    break;
  default:
    cerr << "Unexpected user mode exception" << (int)which << "\n";
    break;
  }
  ASSERTNOTREACHED();
}
