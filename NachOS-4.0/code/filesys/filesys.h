// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"
#define MaxFileOpen 20
#define reverseFD 2


struct OpenFileSocket
{
	int fd;
	OpenFileSocket(int fd1)
	{
		fd = fd1;
	}
	~OpenFileSocket()
	{
		CloseSocket(fd);
	}
};

struct FileSlot
{
	int type; // 0: file, 1: socket
	OpenFile* fileOpen;
	OpenFileSocket* fileSocket;
	FileSlot(OpenFile* f){
		fileOpen = f;
		fileSocket = NULL;
		type = 0;
	}
	FileSlot(OpenFileSocket* f){
		fileOpen = NULL;
		fileSocket = f;
		type = 1;
	}
	~FileSlot(){
		if (fileOpen != NULL) delete fileOpen;
		if (fileSocket != NULL) delete fileSocket;
	}
};

#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
				// calls to UNIX, until the real file system
				// implementation is available
class FileSystem {
  public:
  	FileSlot **fileSlot;
    FileSystem()
	{
		fileSlot = new FileSlot*[MaxFileOpen];
		
		for(int i=0;i<MaxFileOpen;i++){
			fileSlot[i] = NULL;
		}
		this->Create("stdin");
		this->Create("stdout");
		fileSlot[0] = new FileSlot(this->Open("stdin",2, 0));
		fileSlot[1] = new FileSlot(this->Open("stdout",3, 1));
	}
	~FileSystem()
	{
		for (int i = 0; i < MaxFileOpen; i++){
			if (fileSlot[i] != NULL)
				delete fileSlot[i];
		}
		delete[] fileSlot;
	}

    bool Create(char *name) {
		
		int fileDescriptor = OpenForWrite(name);

		if (fileDescriptor == -1) return FALSE;
		Close(fileDescriptor); 
		return TRUE; 
	}
	// NEW OPEN
	OpenFile *Open(char *name)
	{
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1)
			return NULL;

		int id = FindFreeSlot();

		if (id == -1) return NULL;

		fileSlot[id] = new FileSlot(new OpenFile(fileDescriptor, name, id));

		return fileSlot[id]->fileOpen;
	}

    OpenFile* Open(char *name, int id) {
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1) return NULL;
		return new OpenFile(fileDescriptor, id);
    }
	// Overload openfile with type
	OpenFile *Open(char *name, int type, int id)
	{
		int fileDescriptor;
		if (type == 4){
			fileDescriptor = OpenForWrite(name);
		}else{
			fileDescriptor = OpenForReadWrite(name, FALSE);
		}

		if (fileDescriptor == -1) return NULL;
		
		return new OpenFile(fileDescriptor, type, name, id);
	}

	int FindFreeSlot(){
		for (int i=reverseFD; i<MaxFileOpen; ++i){
			if (fileSlot[i] == NULL) return i;
		}
		return -1;
	}

    bool Remove(char *name) { return Unlink(name) == 0; }

	// Part 2: Socket programming

	int createTCP()
	{
		int slot = FindFreeSlot();
		if (slot == -1)
			return -1;
		int fd = OpenSocketInternet();
		//int b = OpenSocket();
		if (fd < -1)
			return -1;
		fileSlot[slot] = new FileSlot(new OpenFileSocket(fd));
		return slot;
	}

	int connectTCP(int socketid, char *ip, int port)
	{
		//cerr << "socketid trong filesys.h " << socketid << endl;
		if (socketid < reverseFD || socketid >= MaxFileOpen)
			return -1;
		if (fileSlot[socketid] == NULL)
			return -1;
		
		return ConnectTCP(fileSlot[socketid]->fileSocket->fd, ip, port);
	}

	int sendTCP(int socketid, char *buffer, int len)
	{
		if (socketid < reverseFD || socketid >= MaxFileOpen)
			return -1;
		if (fileSlot[socketid] == NULL)
			return 0;
		//cerr << "GO HERE" << endl;
		return Send(fileSlot[socketid]->fileSocket->fd, buffer, len);
	}

	int receiveTCP(int socketid, char *buffer, int len)
	{
		if (socketid < reverseFD || socketid >= MaxFileOpen)
			return -1;
		if (fileSlot[socketid] == NULL)
			return 0;
		return Receive(fileSlot[socketid]->fileSocket->fd, buffer, len);
	}

	int closeTCP(int socketid)
	{
		if (socketid < reverseFD || socketid >= MaxFileOpen)
			return -1;
		if (fileSlot[socketid] == NULL)
			return -1;
		CloseSocket(fileSlot[socketid]->fileSocket->fd);
		delete fileSlot[socketid];
		fileSlot[socketid] == NULL;
		return 0;
	}
	/// project 2
	

	int CloseFile(int id) {
		if (id < 2 || id >= MaxFileOpen)
			return -1;
		if (fileSlot[id] == NULL)
			return -1;
		delete fileSlot[id];
		fileSlot[id] = NULL;
		return 0;
	}
};

#else // FILESYS
class FileSystem {
  public:
  	FileSlot **fileSlot;
    FileSystem(bool format);		// Initialize the file system.
					// Must be called *after* "synchDisk" 
					// has been initialized.
    					// If "format", there is nothing on
					// the disk, so initialize the directory
    					// and the bitmap of free blocks.
    
	bool Create(char *name);  	
    bool Create(char *name, int initialSize);  	
					// Create a file (UNIX creat)

    OpenFile* Open(char *name); 	// Open a file (UNIX open)
	OpenFile* Open(char *name, int type); 	// Openfile with type
    int FindFreeSlot();

    bool Remove(char *name);  		// Delete a file (UNIX unlink)

    void List();			// List all the files in the file system

    void Print();			// List all the files and their contents

  private:
   OpenFile* freeMapFile;		// Bit map of free disk blocks,
					// represented as a file
   OpenFile* directoryFile;		// "Root" directory -- list of 
					// file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
