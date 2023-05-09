#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"
#include "synch.h"

#define MAX_PROCESS 10

class PTable
{
private:
	int psize;
	Bitmap *bm;               
	PCB* pcb[MAX_PROCESS];

	Semaphore* bmsem;         

public:
     PTable(int size = 10);          
    ~PTable();             
		
    int ExecUpdate(char* name);
    int ExitUpdate(int ec);
    int JoinUpdate(int id);  

    int GetFreeSlot();       
    bool IsExist(int pid);  
    
    void Remove(int pid);      

    char* GetFileName(int id); 
};
#endif

