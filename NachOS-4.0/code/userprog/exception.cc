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
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"

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
//	"which" is the kind of exception.  The list of possible exceptions
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
void handle_SC_SocketTCP_Send();
void handle_SC_SocketTCP_Receive();
void handle_SC_SocketTCP_Close();

void handle_SC_Exec();
void handle_SC_JOIN();
void handle_SC_EXIT();
void handle_SC_CreateSemaphore();
void handle_SC_Wait();
void handle_SC_Signal();


// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to S    case SC_Add:

char *User2System(int virtAddr, int limit)
{
	int i; // index
	int oneChar;
	char *kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; // need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	// printf("\n Filename u2s:");
	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		// printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

/* Modify return point */
void PCIncrease()
{
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 8);
}

void handle_SC_Add()
{
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
					/* int op2 */ (int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	return PCIncrease();
}

void handle_SC_Create()
{
	// cerr << "CREATE?>>>?\n";
	int virtAddr;
	char *filename;
	DEBUG('a', "\n SC_Create call ...");
	DEBUG('a', "\n Reading virtual address of filename");
	// Lấy tham số tên tập tin từ thanh ghi r4
	virtAddr = kernel->machine->ReadRegister(4);
	DEBUG('a', "\n Reading filename.");
	// MaxFileLength là = 32
	filename = User2System(virtAddr, MaxFileLength + 1);
	if (filename == NULL)
	{
		printf("\n Not enough memory in system");
		DEBUG('a', "\n Not enough memory in system");
		kernel->machine->WriteRegister(2, -1); // trả về lỗi cho chương
		// trình người dùng
		delete[] filename;

		return PCIncrease();
	}
	DEBUG('a', "\n Finish reading filename.");
	// DEBUG('a',"\n File name : '"<<filename<<"'");
	//  Create file with size = 0
	//  Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
	//  việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
	//  hành Linux, chúng ta không quản ly trực tiếp các block trên
	//  đĩa cứng cấp phát cho file, việc quản ly các block của file
	//  trên ổ đĩa là một đồ án khác
	if (!kernel->fileSystem->Create(filename))
	{
		printf("\n Error create file '%s'", filename);
		kernel->machine->WriteRegister(2, -1);
		delete[] filename;

		return PCIncrease();
	}
	kernel->machine->WriteRegister(2, 0); // trả về cho chương trình
	// người dùng thành công
	delete[] filename;
	return PCIncrease();
}

void handle_SC_Open()
{
	// cerr << "OPEN FILE" << endl;
	//  Mở console io 2 lần lien tiếp thì sao ?
	int virAddr = kernel->machine->ReadRegister(4);
	int type = kernel->machine->ReadRegister(5);
	char *filename;
	filename = User2System(virAddr, MaxFileLength);
	int freeSlot = kernel->fileSystem->FindFreeSlot();
	if (freeSlot != -1) // Process when empty slot exists
	{
		if (type == _ReadWrite || type == _ReadOnly || type == _WriteNEW)
		{
			if ((kernel->fileSystem->fileSlot[freeSlot] = new FileSlot(kernel->fileSystem->Open(filename, type))) != NULL)
			{ // Sucessful
				kernel->machine->WriteRegister(2, freeSlot);
			}
			// cerr << "OPEN " << filename <<  "   ::: SLOT    " << freeSlot << endl;
		}
		else if (type == 2)
		{ // 2: stdin
			kernel->machine->WriteRegister(2, 0);
		}
		else
		{ // 3: stdout
			kernel->machine->WriteRegister(2, 1);
		}
		delete[] filename;

		return PCIncrease();
	}
	else
	{
		// cerr << "HET SLOT" << endl;
	}
	kernel->machine->WriteRegister(2, -1);
	delete[] filename;

	return PCIncrease();
}

void handle_SC_Close()
{
	// cerr << "CLOSE" << endl;
	int fileID = kernel->machine->ReadRegister(4);
	if (fileID >= 2 && fileID < MaxFileOpen)
	{
		if (kernel->fileSystem->fileSlot[fileID] != NULL)
		{
			// ADVANCED: If read at socket go to receive
			// cerr << kernel->fileSystem->fileSlot[fileID]->type << endl;
			if (kernel->fileSystem->fileSlot[fileID]->type == 1)
			{
				int rVal;

				rVal = kernel->fileSystem->closeTCP(fileID);

				kernel->machine->WriteRegister(2, rVal);
				return PCIncrease();
			}
			////////////////////////////////////////////////////
			delete kernel->fileSystem->fileSlot[fileID];
			kernel->fileSystem->fileSlot[fileID] = NULL;
			kernel->machine->WriteRegister(2, 0);

			return PCIncrease();
		}
	}
	kernel->machine->WriteRegister(2, -1);
	return PCIncrease();
}

void handle_SC_Read()
{
	// cerr << "READ" << endl;
	int virAddr = kernel->machine->ReadRegister(4);
	int charCnt = kernel->machine->ReadRegister(5);
	// cerr << "-- So luong  " << charCnt << endl;
	int id = kernel->machine->ReadRegister(6);
	int oldPosition, newPosition;
	char *buf;
	if (id < 0 || id >= MaxFileOpen)
	{
		// cerr<<"Read failed\n";
		kernel->machine->WriteRegister(2, -1);
		return PCIncrease();
	}
	if (kernel->fileSystem->fileSlot[id] == NULL)
	{
		// cerr<<"File not exited\n";
		kernel->machine->WriteRegister(2, -1);
		return PCIncrease();
	}
	// ADVANCED: If read at socket go to receive
	if (kernel->fileSystem->fileSlot[id]->type == 1)
	{
		// COPY TU RECEIVE

		char *buffer = new char[charCnt];

		int rVal = kernel->fileSystem->receiveTCP(id, buffer, charCnt);

		if (rVal == -1)
		{
			// printf("\n Failed to send data");
		}
		else if (rVal == 0)
		{
			// printf("\n Connection closed");
		}
		else
		{
			// printf("Successfully recieve %d bytes of data", rVal);
			rVal = System2User(virAddr, rVal, buffer);
		}

		kernel->machine->WriteRegister(2, rVal);
		delete[] buffer;
		return PCIncrease();
	}
	////////////////////////////////////////////////////
	if (kernel->fileSystem->fileSlot[id]->fileOpen->type == 3)
	{
		// cerr<"Cannot read stdout\n";
		kernel->machine->WriteRegister(2, -1);
		return PCIncrease();
	}
	oldPosition = kernel->fileSystem->fileSlot[id]->fileOpen->GetCurrentPos();
	buf = User2System(virAddr, charCnt);
	if (kernel->fileSystem->fileSlot[id]->fileOpen->type == 2)
	{
		int size = 0;
		char t, *tmp = new char[charCnt + 1];
		// cerr << size << ' ' << charCnt << endl;
		while (size < charCnt)
		{
			t = kernel->synchConsoleIn->GetChar();
			// cerr << size << ": " << t << endl;
			if (t == EOF || t == '\n' || t == '\0' || t == 0)
				break;
			tmp[size] = t;
			size++;
		}
		tmp[size] = '\0';
		buf = tmp;
		System2User(virAddr, size + 1, buf);
		kernel->machine->WriteRegister(2, size);
		delete[] buf;
		return PCIncrease();
	}
	if ((kernel->fileSystem->fileSlot[id]->fileOpen->Read(buf, charCnt)) > 0)
	{
		// cerr << charCnt << endl;
		newPosition = kernel->fileSystem->fileSlot[id]->fileOpen->GetCurrentPos();
		System2User(virAddr, newPosition - oldPosition, buf);
		// cerr << buf << endl;
		kernel->machine->WriteRegister(2, newPosition - oldPosition);
		// cerr << "READ SUCCESS FULL" << endl;
	}
	else
	{
		kernel->machine->WriteRegister(2, 0);
	}
	delete[] buf;
	return PCIncrease();
}

void handle_SC_Write()
{
	// cerr << "SC WRITE !!!" << endl;
	int virAddr = kernel->machine->ReadRegister(4);
	int charCnt = kernel->machine->ReadRegister(5);
	int id = kernel->machine->ReadRegister(6);
	int oldPosition;
	int newPosition;
	char *buf;
	if (id < 0 || id >= MaxFileOpen)
	{
		// cerr<<"Outside file table\n";
		kernel->machine->WriteRegister(2, -1);
		return PCIncrease();
	}
	if (kernel->fileSystem->fileSlot[id] == NULL)
	{
		// cerr<<"Can't open file\n";
		kernel->machine->WriteRegister(2, -1);
		return PCIncrease();
	}
	// ADVANCED: If read at socket go to receive
	if (kernel->fileSystem->fileSlot[id]->type == 1)
	{
		buf = User2System(virAddr, charCnt);
		// cerr << "SEND excep " << buf << ' ' << charCnt<<endl;

		if (buf == NULL)
		{
			kernel->machine->WriteRegister(2, -1);
			delete[] buf;
			return PCIncrease();
		}

		int rVal = kernel->fileSystem->sendTCP(id, buf, charCnt);

		// DEBUG
		if (rVal == -1)
		{
			// printf("\n Failed to send data");
		}
		else if (rVal == 0)
		{
			// printf("\n Connection closed");
		}
		else
		{
			// printf("Successfully sent %d bytes of data", rVal);
		}
		kernel->machine->WriteRegister(2, rVal);
		delete[] buf;
		return PCIncrease();
	}
	////////////////////////////////////////////////////

	if (kernel->fileSystem->fileSlot[id]->fileOpen->type == 1 || kernel->fileSystem->fileSlot[id]->fileOpen->type == 2)
	{
		// cerr<<"\nCan't write readonly file or stdin file\n";
		kernel->machine->WriteRegister(2, -1);
		return PCIncrease();
	}
	oldPosition = kernel->fileSystem->fileSlot[id]->fileOpen->GetCurrentPos();

	buf = User2System(virAddr, charCnt);

	if (kernel->fileSystem->fileSlot[id]->fileOpen->type == 0 || kernel->fileSystem->fileSlot[id]->fileOpen->type == _WriteNEW)
	{
		if (kernel->fileSystem->fileSlot[id]->fileOpen->Write(buf, charCnt) > 0)
		{
			newPosition = kernel->fileSystem->fileSlot[id]->fileOpen->GetCurrentPos();
			kernel->machine->WriteRegister(2, newPosition - oldPosition);
			delete[] buf;
			return PCIncrease();
		}
	}
	if (kernel->fileSystem->fileSlot[id]->fileOpen->type == 3)
	{
		// STDOUT
		int i = 0;
		while (i < charCnt && buf[i] != '\0' && buf[i] != 0)
		{
			kernel->synchConsoleOut->PutChar(buf[i]);
			i++;
		}
		buf[i] = '\n';
		kernel->synchConsoleOut->PutChar(buf[i]);
		kernel->machine->WriteRegister(2, i);
		delete[] buf;
		return PCIncrease();
	}
	delete[] buf;

	return PCIncrease();
}

void handle_SC_Seek()
{
	int position = kernel->machine->ReadRegister(4);
	int id = kernel->machine->ReadRegister(5);
	// cerr << "SEEK " << position << " file id: " << id << endl;
	if (id < 0 || id >= MaxFileOpen)
	{
		// cerr<<"-- Outside file table\n";
		kernel->machine->WriteRegister(2, -1);

		return PCIncrease();
	}
	if (kernel->fileSystem->fileSlot[id] == NULL)
	{
		// cerr<<"-- File not exists\n";
		kernel->machine->WriteRegister(2, -1);

		return PCIncrease();
	}
	if (id == 0 || id == 1)
	{
		// cerr<<"-- Cannot call seek on console\n";
		kernel->machine->WriteRegister(2, -1);

		return PCIncrease();
	}
	if (position == -1)
	{
		position = kernel->fileSystem->fileSlot[id]->fileOpen->Length();
		// cerr << "-- new length " << position << endl;
	}
	if (position > kernel->fileSystem->fileSlot[id]->fileOpen->Length() || position < 0)
	{
		// cerr<<"Cannot seek to this position";
		kernel->machine->WriteRegister(2, -1);
	}
	else
	{
		kernel->fileSystem->fileSlot[id]->fileOpen->Seek(position);
		kernel->machine->WriteRegister(2, position);
	}
	return PCIncrease();
}

void handle_SC_Remove()
{
	// cerr << "vao remove" << endl;
	int virtAdr = kernel->machine->ReadRegister(4);
	char *filename;
	filename = User2System(virtAdr, MaxFileLength + 1);
	for (int i = 0; i < MaxFileOpen; i++)
		if (kernel->fileSystem->fileSlot[i] != NULL)
		{
			if (strcmp(kernel->fileSystem->fileSlot[i]->fileOpen->fName, filename) == 0)
			{
				// cerr<<"Error, file opening";
				kernel->machine->WriteRegister(2, -1);
				delete[] filename;

				return PCIncrease();
			}
		}
	int ans = kernel->fileSystem->Remove(filename);
	if (!ans)
	{
		// cerr << "Something went wrong in remove" << endl;
		kernel->machine->WriteRegister(2, -1);
	}
	else
	{
		// cerr << "Remove successful" << endl;
		kernel->machine->WriteRegister(2, 0);
	}
	delete[] filename;

	return PCIncrease();
}

void handle_SC_SocketTCP_Open()
{
	DEBUG('a', "\n SC_OpenSocket Calls......");
	int socketID;
	socketID = kernel->fileSystem->createTCP();
	// cerr << "socketID: " << kernel->fileSystem->fileSocket[socketID] << endl;
	if (socketID == -1)
	{
		// printf("\n Error create Socket....");
		kernel->machine->WriteRegister(2, -1);
		PCIncrease();
		return;
	}

	// printf("\n Successfully creating socket for system, socketID: %d\n", socketID);
	kernel->machine->WriteRegister(2, socketID);
	return PCIncrease();
}

void handle_SC_SocketTCP_Connect()
{
	DEBUG('a', "\n SC_ConnectSocket Calls......\n");
	int socketID, vAddress, port;
	char *ip;

	DEBUG('a', "\n Reading SocketID\n");
	socketID = kernel->machine->ReadRegister(4);

	DEBUG('a', "\n Reading virtual address\n");
	vAddress = kernel->machine->ReadRegister(5);

	DEBUG('a', "\n Reading IP\n");
	ip = User2System(vAddress, MAXIpAddressLength);

	if (ip == NULL)
	{
		kernel->machine->WriteRegister(2, -1);
		delete[] ip;

		return PCIncrease();
	}

	DEBUG('a', "\n Reading port\n");
	port = kernel->machine->ReadRegister(6);

	// cerr << ip << ' ' << socketID << ' ' << port << endl;

	if (kernel->fileSystem->connectTCP(socketID, ip, port) == -1)
	{
		// printf("\nFailed to connect to SocketId: %d, IP: %s, Port: %d\n", socketID, ip, port);
		kernel->machine->WriteRegister(2, -1);
		delete[] ip;
		return PCIncrease();
	}

	// printf("\n Sucessfully connect to SocketId: %d, IP: %s, Port: %d\n", socketID, ip, port);
	kernel->machine->WriteRegister(2, 0);
	delete[] ip;
	return PCIncrease();
}

void handle_SC_SocketTCP_Send()
{
	int socketID, leng, vAddress, rVal;
	char *buff;

	DEBUG('a', "\n SC_Send calls....");
	DEBUG('a', "\n Reading SocketID");
	socketID = kernel->machine->ReadRegister(4);

	DEBUG('a', "\n Reading virtual address of buffer.....");
	vAddress = kernel->machine->ReadRegister(5);

	DEBUG('a', "\n Reading buffer");
	buff = User2System(vAddress, leng);
	cerr << "SOCKET: " << buff << endl;

	if (buff == NULL)
	{
		DEBUG('a', "\n Not enough memory for our system");
		kernel->machine->WriteRegister(2, -1);
		delete[] buff;
		return PCIncrease();
	}
	cerr << buff << endl;

	DEBUG('a', "\n Finish reading buffer.");

	DEBUG('a', "\n Reading length");
	leng = kernel->machine->ReadRegister(6);

	rVal = kernel->fileSystem->sendTCP(socketID, buff, leng);

	// DEBUG
	if (rVal == -1)
	{
		// printf("\n Failed to send data");
	}
	else if (rVal == 0)
	{
		// printf("\n Connection closed");
	}
	else
	{
		// printf("Successfully sent %d bytes of data", rVal);
	}
	kernel->machine->WriteRegister(2, rVal);
	delete[] buff;
	return PCIncrease();
}

void handle_SC_SocketTCP_Receive()
{
	int socketID, len, vAddress, rVal;
	char *buffer;

	socketID = kernel->machine->ReadRegister(4);
	vAddress = kernel->machine->ReadRegister(5);
	len = kernel->machine->ReadRegister(6);

	buffer = new char[len];

	rVal = kernel->fileSystem->receiveTCP(socketID, buffer, len);

	if (rVal == -1)
	{
		// printf("\n Failed to send data");
	}
	else if (rVal == 0)
	{
		// printf("\n Connection closed");
	}
	else
	{
		// printf("Successfully recieve %d bytes of data", rVal);
		rVal = System2User(vAddress, rVal, buffer);
	}

	kernel->machine->WriteRegister(2, rVal);
	delete[] buffer;
	return PCIncrease();
}

void handle_SC_SocketTCP_Close()
{
	int socketID, rVal;
	DEBUG('a', "\n SC_CloseSocket calls ...");
	DEBUG('a', "\n Reading Socket ID");
	socketID = kernel->machine->ReadRegister(4);

	rVal = kernel->fileSystem->closeTCP(socketID);

	kernel->machine->WriteRegister(2, rVal);
	return PCIncrease();
}
/// @brief /////////////////////////////////
void handle_SC_EXEC(){
	int vAddr;
	vAddr = kernel -> machine -> ReadRegister(4);
	char* name;
	name = User2System(vAddr, MaxFileLength);
	if(name == NULL){
		DEBUG('a', "\n Not enough memory in System");
		printf("\n Not enough memory in System");
		kernel -> machine -> WriteRegister(2, -1);
		return PCIncrease();
	}
	OpenFile * file1 = kernel -> fileSystem -> Open(name);
	if(file1 == NULL){
		printf("\nExec:: Can't open this file.");
		kernel -> machine ->WriteRegister(2,-1);
		return PCIncrease();
	}

	delete file1;
	int n = kernel -> pTab -> ExecUpdate(name);
	kernel -> machine -> WriteRegister(2, n);
	
    // DO NOT DELETE NAME, THE THEARD WILL DELETE IT LATER
	//delete [] name;
	
	return PCIncrease();
}

void handle_SC_JOIN(){
	int n = kernel -> machine -> ReadRegister(4);
	int result = kernel -> pTab -> JoinUpdate(n);

	kernel -> machine -> WriteRegister(2, result);
	return PCIncrease();
}

void handle_SC_EXIT(){
	int eStatus = kernel -> machine -> ReadRegister(4);

	if(eStatus != 0) return PCIncrease();

	int res = kernel -> pTab -> ExitUpdate(eStatus);
	kernel -> currentThread -> FreeSpace();
	kernel -> currentThread -> Finish();
	return PCIncrease();
}

void handle_SC_CreateSemaphore(){
	int vAddr = kernel -> machine -> ReadRegister(4);
	int sVal = kernel -> machine -> ReadRegister(5);

	char* name = User2System(vAddr, MaxFileLength + 1);

	if(name == NULL){
		DEBUG('a', "\n Not enough memory in System");
		printf("\n Not enough memory in System");
		kernel -> machine -> WriteRegister(2, -1);
		delete[] name;
		return PCIncrease();
	}

	int result = kernel -> semTab -> Create(name, sVal);
	
	if(result == -1){
		DEBUG('a', "\n Cannot create semaphore");
		printf("\n Cannot create semaphore");
		kernel -> machine -> WriteRegister(2, -1);
		delete [] name;
		return PCIncrease();		
	}
			
	delete [] name;
	kernel -> machine -> WriteRegister(2, result);
	return PCIncrease();
}

void handle_SC_Wait(){
	int vAddr = kernel -> machine -> ReadRegister(4);

	char *name = User2System(vAddr, MaxFileLength + 1);
	if(name == NULL){
		DEBUG('a', "\n Not enough memory in System");
		printf("\n Not enough memory in System");
		kernel -> machine -> WriteRegister(2, -1);
		delete[] name;
		return PCIncrease();
	}
			
	int result = kernel -> semTab -> Wait(name);

	if(result == -1){
		DEBUG('a', "\n This semaphore does not exist!");
		printf("\n This semaphore does not exist!");
		kernel -> machine->WriteRegister(2, -1);
		delete[] name;
		return PCIncrease();		
	}
			
	delete[] name;
	kernel -> machine->WriteRegister(2, result);
	return PCIncrease();
}

void handle_SC_Signal(){
	int vAddr = kernel -> machine -> ReadRegister(4);

	char *name = User2System(vAddr, MaxFileLength + 1);
	if(name == NULL){
		DEBUG('a', "\n Not enough memory in System");
		printf("\n Not enough memory in System");
		kernel -> machine->WriteRegister(2, -1);
		delete[] name;
		return PCIncrease();
	}
			
	int result = kernel -> semTab -> Signal(name);

	if(result == -1){
		DEBUG('a', "\n This semaphore does not exist!");
		printf("\n This semaphore does not exist!");
			kernel -> machine -> WriteRegister(2, -1);
			delete[] name;
			return PCIncrease();				
	}
			
	delete[] name;
	kernel -> machine -> WriteRegister(2, result);
	return PCIncrease();
}

void handle_SC_EXECV(){
	
}



void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);
	// cerr << which << ' ' << type << endl;

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;

		case SC_Add:
			return handle_SC_Add();
		case SC_Create:
			return handle_SC_Create();
		case SC_Open:
			return handle_SC_Open();
		case SC_Close:
			return handle_SC_Close();
		case SC_Read:
			return handle_SC_Read();
		case SC_Write:
			return handle_SC_Write();
		case SC_Seek:
			return handle_SC_Seek();
		case SC_Remove:
			return handle_SC_Remove();
		// SOCKET
		case SC_SocketTCP_Open:
			return handle_SC_SocketTCP_Open();
		case SC_SocketTCP_Connect:
			return handle_SC_SocketTCP_Connect();
		case SC_SocketTCP_Send:
			return handle_SC_SocketTCP_Send();
		case SC_SocketTCP_Receive:
			return handle_SC_SocketTCP_Receive();
		case SC_SocketTCP_Close:
			return handle_SC_SocketTCP_Close();

		// Multiprogramming
		case SC_Exec:
			return handle_SC_EXEC();
		case SC_Join:
			return handle_SC_JOIN();
		case SC_Exit:
			return handle_SC_EXIT();
		case SC_CreateSemaphore:
			return handle_SC_CreateSemaphore();
		case SC_Wait:
			return handle_SC_Wait();
		case SC_Signal:
			return handle_SC_Signal();
		case SC_ExecV:
			return handle_SC_EXECV();

		default:
			cerr << "Unexpected system call " << type << "\n";
			kernel->currentThread->Finish();
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception " << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
