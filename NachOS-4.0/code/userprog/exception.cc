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
#include"sysdep.h"
#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
const int MAXIpAddressLength = 15;
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


// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to S
char* User2System(int virtAddr,int limit) 
{ 
	int i;// index 
	int oneChar; 
	char* kernelBuf = NULL; 
	kernelBuf = new char[limit +1];//need for terminal string 
	if (kernelBuf == NULL) 
	return kernelBuf; 
	memset(kernelBuf,0,limit+1); 
	//printf("\n Filename u2s:"); 
	for (i = 0 ; i < limit ;i++) 
	{ 
	kernel->machine->ReadMem(virtAddr+i,1,&oneChar); 
	kernelBuf[i] = (char)oneChar; 
	//printf("%c",kernelBuf[i]); 
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
int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0;
	do{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

/* Modify return point */
void PCIncrease() {
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
}

void handle_SC_Add(){
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	return PCIncrease();
}

void handle_SC_Create(){
	int virtAddr;
	char* filename;
	DEBUG('a',"\n SC_Create call ...");
	DEBUG('a',"\n Reading virtual address of filename");
	// Lấy tham số tên tập tin từ thanh ghi r4
	virtAddr = kernel->machine->ReadRegister(4);
	DEBUG ('a',"\n Reading filename.");
	// MaxFileLength là = 32
	filename = User2System(virtAddr, MaxFileLength +1);
	if (filename == NULL)
	{
		printf("\n Not enough memory in system");
		DEBUG('a',"\n Not enough memory in system");
		kernel->machine->WriteRegister(2,-1); // trả về lỗi cho chương
		// trình người dùng
		delete filename;

		return PCIncrease();
	}
	DEBUG('a',"\n Finish reading filename.");
	//DEBUG('a',"\n File name : '"<<filename<<"'");
	// Create file with size = 0
	// Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
	// việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
	// hành Linux, chúng ta không quản ly trực tiếp các block trên
	// đĩa cứng cấp phát cho file, việc quản ly các block của file
	// trên ổ đĩa là một đồ án khác
	if (!kernel->fileSystem->Create(filename))
	{
		printf("\n Error create file '%s'",filename);
		kernel->machine->WriteRegister(2,-1);
		delete filename;

		return PCIncrease();
	}
	kernel->machine->WriteRegister(2,0); // trả về cho chương trình
	// người dùng thành công
	delete filename;
	return PCIncrease();
}

void handle_SC_Open(){
	int virAddr = kernel->machine->ReadRegister(4); 
	int type = kernel->machine->ReadRegister(5); 
	char* filename;
	filename = User2System(virAddr, MaxFileLength);			
	int freeSlot = kernel->fileSystem->FindFreeSlot();
	if (freeSlot != -1) //Process when empty slot exists
	{
		if (type == 0 || type == 1) 
		{
			if ((kernel->fileSystem->openingFile[freeSlot] = kernel->fileSystem->Open(filename, type)) != NULL) //Sucessful
				kernel->machine->WriteRegister(2, freeSlot);
		}
		else if (type == 2) // stdin
			kernel->machine->WriteRegister(2, 0); 
		else // stdout
			kernel->machine->WriteRegister(2, 1); 
		delete[] filename;
		
		return PCIncrease();
	}
	kernel->machine->WriteRegister(2, -1); 
	delete[] filename;
	
	return PCIncrease();
}

void handle_SC_Close(){
	int fileID=kernel->machine->ReadRegister(4);
	if(fileID>=0&&fileID<=MaxFile)
		if(kernel->fileSystem->openingFile[fileID]){
			delete kernel->fileSystem->openingFile[fileID];
			kernel->fileSystem->openingFile[fileID]=NULL;
			kernel->machine->WriteRegister(2,0);
			PCIncrease();
			return;
		}
	kernel->machine->WriteRegister(2,-1);
	return PCIncrease();
}

void handle_SC_Read(){
	int virAddr=kernel->machine->ReadRegister(4);
	int charCnt=kernel->machine->ReadRegister(5);
	int id=kernel->machine->ReadRegister(6);
	int oldPosition, newPosition;
	char* buf;
	if(id<0||id>MaxFile) {
		//cerr<<"Read failed\n";
		kernel->machine->WriteRegister(2,-1);
		return PCIncrease();
	}
	if(kernel->fileSystem->openingFile[id]==NULL){
		//cerr<<"File not exited\n";
		kernel->machine->WriteRegister(2,-1);
		return PCIncrease();
	}
	if(kernel->fileSystem->openingFile[id]->t==3){
		//cerr<"Cannot print stdout\n";
		kernel->machine->WriteRegister(2,-1);
		return PCIncrease();
	}
	oldPosition=kernel->fileSystem->openingFile[id]->GetCurrentPos();
	buf=User2System(virAddr, charCnt);
	if(kernel->fileSystem->openingFile[id]->t==2){
		int size=0;
		char t=NULL, *tmp=new char[charCnt+1];
		while(size<charCnt){
			t=kernel->synchConsoleIn->GetChar();
			tmp[size]=t;
			if(t=='\n') break;
			size++;
		}
		tmp[size+1]='\0';
		buf=tmp;
		System2User(virAddr,size, buf);
		kernel->machine->WriteRegister(2,size);
		delete buf, tmp, t;
		return PCIncrease();
	}
	if((kernel->fileSystem->openingFile[id]->Read(buf, charCnt))>0){
		newPosition=kernel->fileSystem->openingFile[id]->GetCurrentPos();
		System2User(virAddr, newPosition-oldPosition, buf);
		kernel->machine->WriteRegister(2, newPosition-oldPosition);
	}
	else kernel->machine->WriteRegister(2,0);
	delete buf;
	return PCIncrease();
}

void handle_SC_Write(){
	int virAddr=kernel->machine->ReadRegister(4);
	int charCnt=kernel->machine->ReadRegister(5);
	int id=kernel->machine->ReadRegister(6);
	int oldPosition;
	int newPosition;
	char *buf;
	if(id<0||id>MaxFile){
		//cerr<<"Outside file table\n";
		kernel->machine->WriteRegister(2,-1);
		return PCIncrease();
	}
	if(kernel->fileSystem->openingFile[id]==NULL){
		//cerr<<"Can't open file\n";
		kernel->machine->WriteRegister(2,-1);
		return PCIncrease();
	}
	if (kernel->fileSystem->openingFile[id]->t == 1 || kernel->fileSystem->openingFile[id]->t == 2)
	{
		printf("\nCan't open readonly file or stdin file");
		kernel->machine->WriteRegister(2, -1);
		return PCIncrease();
	}
	oldPosition=kernel->fileSystem->openingFile[id]->GetCurrentPos();
	buf=User2System(virAddr, charCnt);
	if(kernel->fileSystem->openingFile[id]->t==0){
		if(kernel->fileSystem->openingFile[id]->Write(buf,charCnt)>0){
			newPosition=kernel->fileSystem->openingFile[id]->GetCurrentPos();
			kernel->machine->WriteRegister(2,newPosition-oldPosition);
			delete buf;
			return PCIncrease();
		}
	}
	if(kernel->fileSystem->openingFile[id]->t==3){
		int i=0;
		while(buf[i]!=0&&buf[i]!='\n'){
			kernel->synchConsoleOut->PutChar(buf[i]);
			i++;
		}
		buf[i]='\n';
		kernel->synchConsoleOut->PutChar(buf[i]);
		kernel->machine->WriteRegister(2,i-1);
		delete buf;
		return PCIncrease();
	}

	return PCIncrease();
}

void handle_SC_Seek(){
	int position=kernel->machine->ReadRegister(4);
	int id=kernel->machine->ReadRegister(5);
	if(id<0||id>MaxFile) {
		//cerr<<"Outside file table\n";
		kernel->machine->WriteRegister(2,-1);
		
		return PCIncrease();
	}
	if(kernel->fileSystem->openingFile==NULL){
		//cerr<<"File not exists\n";
		kernel->machine->WriteRegister(2,-1);

		return PCIncrease();
	}
	if(id==0||id==1){
		//cerr<<"Cannot call seek on console\n";
		kernel->machine->WriteRegister(2,-1);
		
		return PCIncrease();
	}
	if(position==-1) position=kernel->fileSystem->openingFile[id]->Length();
	else position=position;
	if (position > kernel->fileSystem->openingFile[id]->Length() || position < 0)
	{
		//cerr<<"Cannot seek to this position";
		kernel->machine->WriteRegister(2, -1);
	}
	else
	{
		kernel->fileSystem->openingFile[id]->Seek(position);
		kernel->machine->WriteRegister(2, position);
	}
	return PCIncrease();
}

void handle_SC_Remove(){			
	int virtAdr=kernel->machine->ReadRegister(4);
	char* filename;
	filename=User2System(virtAdr, MaxFileLength+1);
	for(int i=0;i<MaxFile;i++){
		if(strcmp(kernel->fileSystem->openingFile[i]->fName, filename)==0){
//			//cerr<<"Error, file opening";
			kernel->machine->WriteRegister(2,-1);
			delete filename;
			PCIncrease();
			return;
		}
	}
	int ans=kernel->fileSystem->Remove(filename);
	if(!ans)
		kernel->machine->WriteRegister(2,-1);
	else kernel->machine->WriteRegister(2,0);
	delete filename;

	return PCIncrease();
}




void openSystemSocket(){
	DEBUG('a', "\n SC_OpenSocket Calls......");
	int socketID;
	if(socketID == kernel -> fileSystem ->createTCP() == -1){
		printf("\n Error create Socket....");
		kernel -> machine ->WriteRegister(2, -1);
		PCIncrease();
		return;
	}

	printf("\n Successfully creating socket for system, socketID: %d", socketID);
	kernel -> machine -> WriteRegister(2, socketID);
	PCIncrease();
}

void connectSystemSocket(){
	DEBUG('a', "\n SC_ConnectSocket Calls......");
	int socketID, vAddress, port;
	char *ip;
	
	DEBUG('a', "\n Reading SocketID");
	socketID = kernel -> machine -> ReadRegister(4);

	DEBUG('a', "\n Reading virtual address");
	vAddress = kernel -> machine -> ReadRegister(5);

	DEBUG('a', "\n Reading IP");
	ip = User2System(vAddress, MAXIpAddressLength);

	if(ip == nullptr){
		kernel -> machine -> WriteRegister(2, -1);
		delete ip;
		PCIncrease();
		return;
	}

	DEBUG('a', "\n Reading port");
	port = kernel -> machine -> ReadRegister(6);

	if(kernel -> fileSystem -> connectTCP(socketID, ip, port) == -1){
		printf("\n Failed to connect to SocketId: %d, IP: %s, Port: %d", socketID, ip, port);
    	kernel->machine->WriteRegister(2, -1);
    	delete ip;
    	PCIncrease();
    	return;
	}

	printf("\n Sucessfully connect to SocketId: %d, IP: %s, Port: %d", socketID, ip, port);
	kernel -> machine -> WriteRegister(2, 0);
	delete ip;
	PCIncrease();
}

void sendSystemSocket(){
	int socketID, leng, vAddress, rVal;
	char *buff;

	DEBUG('a', "\n SC_Send calls....");
	DEBUG('a', "\n Reading SocketID");
	socketID = kernel -> machine -> ReadRegister(4);
	
	DEBUG('a', "\n Reading virtual address of buffer.....");
	vAddress = kernel -> machine -> ReadRegister(5);

	DEBUG('a', "\n Reading buffer");
	buff = User2System(vAddress, leng);

	if(buff == nullptr){
		DEBUG('a', "\n Not enough memory for our system");
		kernel -> machine -> WriteRegister(2, -1);
		delete buff;
		PCIncrease();
		return;
	}

	  DEBUG('a', "\n Finish reading buffer.");

  	DEBUG('a', "\n Reading length");
  	leng = kernel->machine->ReadRegister(6);

	rVal = kernel -> fileSystem -> sendTCP(socketID, buff, leng);

	if(rVal == -1) printf("\n Failed to send data");
	else if(rVal == 0) printf("\n Connection closed");
	else printf("Successfully sent %d bytes of data", rVal);

	kernel -> machine -> WriteRegister(2, rVal);
	delete buff;
	PCIncrease();
}


void receiveSystemSocket(){
	int socketID, leng, vAddress, rVal;
	char * buff;
	DEBUG('a', "\n SC_Receive call....");
	DEBUG('a', "\n Reading Socket ID");
	socketID = kernel -> machine -> ReadRegister(4);

	DEBUG('a', "\n Reading virtual address");
	vAddress = kernel -> machine -> ReadRegister(5);

	DEBUG('a', "\n Reading length");
  	leng = kernel->machine->ReadRegister(6);

	buff = new char[leng];

	rVal = kernel -> fileSystem ->receiveTCP(socketID, buff, leng);

	if(rVal == -1)

	if(rVal == -1) printf("\n Failed to send data");
	else if(rVal == 0) printf("\n Connection closed");
	else {
		printf("Successfully recieve %d bytes of data", rVal);
		rVal = System2User(vAddress, rVal, buff);
	}

	kernel -> machine -> WriteRegister(2, rVal);
  	delete buff;
  	PCIncrease();
}

void systemCloseSocket(){
	int socketID, rVal;
	DEBUG('a', "\n SC_CloseSocket calls ...");
	DEBUG('a', "\n Reading Socket ID");
	socketID = kernel -> machine -> ReadRegister(4);

	rVal = kernel -> fileSystem -> closeTCP(socketID);

	kernel -> machine -> WriteRegister(2, rVal);
	PCIncrease();
}

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
      switch(type) {
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

		case SC_SocketTCP_Open:{
			openSystemSocket();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_SocketTCP_Connect:{
			connectSystemSocket();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_SocketTCP_Send:{
			sendSystemSocket();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_SocketTCP_Receive:{
			receiveSystemSocket();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_SocketTCP_Close:{
			systemCloseSocket();
			return;
			ASSERTNOTREACHED();
			break;
		}

      default:
	cerr << "Unexpected system call " << type << "\n";
	break;
	}
    default:
      cerr << "Unexpected user mode exception: " << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}
