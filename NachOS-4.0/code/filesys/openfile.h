// openfile.h 
//	Data structures for opening, closing, reading and writing to 
//	individual files.  The operations supported are similar to
//	the UNIX ones -- type 'man open' to the UNIX prompt.
//
//	There are two implementations.  One is a "STUB" that directly
//	turns the file operations into the underlying UNIX operations.
//	(cf. comment in filesys.h).
//
//	The other is the "real" implementation, that turns these
//	operations into read and write disk sector requests. 
//	In this baseline implementation of the file system, we don't 
//	worry about concurrent accesses to the file system
//	by different threads.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef OPENFILE_H
#define OPENFILE_H

#include "copyright.h"
#include "utility.h"
#include "sysdep.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "debug.h"


#ifdef FILESYS_STUB			// Temporarily implement calls to 
					// Nachos file system as calls to UNIX!
					// See definitions listed under #else
class OpenFile {
  public:
    OpenFile(int f, int id, char* n) { // file constructor
		file = f; 
		fileDescriptorId = id; 
		currentOffset = 0; 
		name = strdup(n);
		isfile = true;
	}	// open the file

	OpenFile(int s, int id) { // socket constructor
		socket = s;
		fileDescriptorId = id;
		isfile = false;
	}

    ~OpenFile() { 
		if (isfile) {
			Close(file); 
			delete[] name;
		}
		else {
			CloseSocket(socket);
		}
	}			// close the file

    int ReadAt(char *into, int numBytes, int position) { 
		if (isfile) {
			Lseek(file, position, 0); 
			return ReadPartial(file, into, numBytes); 
		}
		ASSERTNOTREACHED();
	}
    int WriteAt(char *from, int numBytes, int position) { 
		if (isfile) {
    		Lseek(file, position, 0); 
			WriteFile(file, from, numBytes); 
			return numBytes;
		}
		ASSERTNOTREACHED();
	}	
    int Read(char *into, int numBytes) {
		if (isfile) {
			int numRead = ReadAt(into, numBytes, currentOffset); 
			currentOffset += numRead;
			return numRead;
		}
		else {
			int bytes_received = recv(socket, into, numBytes, 0);
			if (bytes_received < 0) {
				return -1;
			}
			else if (bytes_received == 0) {
				return 0;
			}
			else {
				return bytes_received;
			}			
		}
		ASSERTNOTREACHED();
    }
    int Write(char *from, int numBytes) {
		if (isfile) {
			int numWritten = WriteAt(from, numBytes, currentOffset); 
			currentOffset += numWritten;
			return numWritten;
		}
		else {
			int bytes_sent = send(socket, from, numBytes, 0);
			if (bytes_sent < 0) {
				return -1;
			}
			else if (bytes_sent == 0) {
				return 0;
			}
			else {
				return bytes_sent;
			}			
		}
		ASSERTNOTREACHED();
	}

	int Connect(char* ip, int port) {
		if (!isfile) {
			struct sockaddr_in serv_addr;
			memset(&serv_addr, '0', sizeof(serv_addr));

			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(port);

			if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
				return -1;
			}

			if (connect(socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
				return -1;
			}

			return 0;
		}
		ASSERTNOTREACHED();
	}

    int Length() { 
		if (isfile) {
			Lseek(file, 0, 2); 
			return Tell(file); 
		}
		ASSERTNOTREACHED();
	}

	int Seek(int position) {
		if (isfile){
			Lseek(file, position, 0);
			currentOffset = Tell(file);
		}
		return currentOffset;
	}
    
	int getFileDescriptorID() { 
		return fileDescriptorId; 
	}

	int getFileID() { 
		return file; 
	}

	char* getName() { 
		return name; 
	}

	bool isSocket() {
		return !isfile;
	}

	bool isFile() {
		return isfile;
	}

  private:
    int file;
	int socket;
	char* name;
	int fileDescriptorId;
    int currentOffset;
	bool isfile;
};

#else // FILESYS
class FileHeader;

class OpenFile {
  public:
    OpenFile(int sector);		// Open a file whose header is located
					// at "sector" on the disk
    ~OpenFile();			// Close the file

    void Seek(int position); 		// Set the position from which to 
					// start reading/writing -- UNIX lseek

    int Read(char *into, int numBytes); // Read/write bytes from the file,
					// starting at the implicit position.
					// Return the # actually read/written,
					// and increment position in file.
    int Write(char *from, int numBytes);

    int ReadAt(char *into, int numBytes, int position);
    					// Read/write bytes from the file,
					// bypassing the implicit position.
    int WriteAt(char *from, int numBytes, int position);

    int Length(); 			// Return the number of bytes in the
					// file (this interface is simpler 
					// than the UNIX idiom -- lseek to 
					// end of file, tell, lseek back 
    
  private:
    FileHeader *hdr;			// Header for this file 
    int seekPosition;			// Current position within the file
};

#endif // FILESYS

#endif // OPENFILE_H
