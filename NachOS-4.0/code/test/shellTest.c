
#include "syscall.h"

int main()
{
    char str1[50], str2[50];
    int write1, tem1, cnt;
    SpaceId newProc1;

    CreateSemaphore("write_std",1);

    while(1){
        Wait("write_std");

        write1 = Read(str1,50,0);
        if (str1[0] == 'q') ++cnt;
        if (str1[1] == 'u') ++cnt;
        if (str1[2] == 'i') ++cnt;
        if (str1[3] == 't') ++cnt;
        if (str1[4] == '\0') ++cnt;

        if(write1==-1 || cnt == 5){
            break;
        }

        Signal("write_std");
        newProc1 = Exec(str1); // Project 01
        tem1 =Join(newProc1);  
    }
}