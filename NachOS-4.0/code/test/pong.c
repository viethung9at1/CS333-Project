#include "syscall.h"

int main()
{
	
	int i;
	for(i =0; i< 5; i++)
	{
		Write("BB", 2, _ConsoleOutput);
    }
	Exit(0);
}