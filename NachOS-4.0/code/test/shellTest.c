#include "syscall.h"
#include "copyright.h" 

int main()
{
    SpaceId newProc1;
    SpaceId newProc2;

    newProc1 = Exec("cat"); // Project 01
    newProc2 = Exec("copy"); // Project 01

    Join(newProc1);
    Join(newProc2);
}

