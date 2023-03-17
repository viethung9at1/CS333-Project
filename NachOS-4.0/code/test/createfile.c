#include "syscall.h" 
#include "copyright.h" 
#define maxlen 32 
int  main(int argc, char* argv[]) { 
    
    if (argc == 1){

    }else
    if (argc == 2){
        Create(argv[1]);
    }else{
        printf("The format is wrong !");
    }

    Halt(); 
} 