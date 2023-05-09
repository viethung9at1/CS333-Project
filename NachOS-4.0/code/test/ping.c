#include "syscall.h"

int main()
{
	
	int i;
	for(i =0; i< 5; i++)
	{
		Write("AAA", 3, _ConsoleOutput);
    }
	Exit(0);
}