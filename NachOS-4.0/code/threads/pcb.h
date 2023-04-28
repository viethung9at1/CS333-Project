#ifndef PCB_H
#define PCB_H

#include "thread.h"
#include "synch.h"


class PCB
{
private:
    Semaphore* joinsem;       
    Semaphore* exitsem;     
    Semaphore* multex;        

    int exitcode;		
    int numwait;                

    char FileName[32];  

    Thread* thread;             
public:
    int parentID;               
    
    char boolBG;                
    
    PCB(int = 0);            
    ~PCB();                 

    int Exec(char*,int);   
    int GetID();            
    int GetNumWait();       


    void JoinWait();         
                        
    void ExitWait();         

    void JoinRelease();       
    void ExitRelease();        

    void IncNumWait();        
    void DecNumWait();       

    void SetExitCode(int);    
    int GetExitCode();       

    void SetFileName(char*);  
    char* GetFileName();   

};

#endif //PCB_H
