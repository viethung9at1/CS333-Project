/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "synchconsole.h"
#include <sys/socket.h>
#include <netinet/in.h>

void SysHalt()
{
    kernel -> interrupt -> Halt();
}


int SysAdd(int op1, int op2)
{
    return op1 + op2;
}

int SysWrite(char* buffer, int charCount, int fileId) {
    if (fileId == 0) return -1;
    if (fileId == 1) return kernel -> synchConsoleOut -> PutString(buffer, charCount);
    if (kernel->fileSystem->getFile(fileId) == NULL) return -1;

    return kernel -> fileSystem -> getFile(fileId) -> Write(buffer, charCount);
}

int SysRead(char* buffer, int charCount, int fileId) {
    if (fileId == 1) return -1;
    if (fileId == 0) return kernel -> synchConsoleIn -> GetString(buffer, charCount);
    if (kernel->fileSystem->getFile(fileId) == NULL) return -1;
    return kernel -> fileSystem -> getFile(fileId) -> Read(buffer, charCount);
}

int SysSeek(int pos, int fileId) {
    return kernel -> fileSystem -> seekFile(pos, fileId);
}

int SysRemove(char* filename) {
    if (kernel -> fileSystem -> checkOpen(filename)) return 0;
    return kernel -> fileSystem -> Remove(filename);
}

int SysSocket() {
    OpenFile* soc = kernel -> fileSystem -> openSocket();
    if (soc == NULL) return -1;
    return soc -> getFileDescriptorID();
}

int SysConnect(int socketid, char *ip, int port) {
    return kernel -> fileSystem -> connect(socketid, ip, port);
}



#endif /* ! __USERPROG_KSYSCALL_H__ */
