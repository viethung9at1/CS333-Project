#include "syscall.h" 
#include "copyright.h" 

int getlengthoffile(int file){
    int len;
    len = Seek(-1, file);
    Seek(0, file);
    return len;
}
void _strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

int main() { 
    int fileId, len;
    char sms[100];
    char* filename;
    char* content;

    _strcpy(sms, "file name:");Write(sms, MaxFileLength, _ConsoleOutput);

    Read(filename, MaxFileLength, _ConsoleInput);
    
    fileId = Open(filename, 1);
    len = getlengthoffile(fileId);
    Read(content, len, fileId);
    
    Write(content, len, _ConsoleOutput);

    Halt(); 
} 