#include "syscall.h" 
#include "copyright.h" 

void _strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

int main() { 
    char* filename;
    char sms[100];

    _strcpy(sms, "file name:");Write(sms, MaxFileLength, _ConsoleOutput);

    Read(filename, MaxFileLength, _ConsoleInput);
    
    Remove(filename);
    Halt(); 
} 