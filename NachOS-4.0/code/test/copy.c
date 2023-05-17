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
    int Id1, len, Id2;
    char fileA[MaxFileLength+1], fileB[MaxFileLength+1], sms[100];
    char* content;

    _strcpy(sms, "file source:");Write(sms, 30, _ConsoleOutput);

    Read(fileA, MaxFileLength, _ConsoleInput);

    _strcpy(sms, "file dest:");Write(sms, 30, _ConsoleOutput);

    Read(fileB, MaxFileLength, _ConsoleInput);
    
    Id1 = Open(fileA, 1);
    Id2 = Open(fileB, _WriteNEW);

    len = getlengthoffile(Id1);

    Read(content, len, Id1);
    Write(content, len, Id2);


    Close(Id1);
    Close(Id2);

    Halt(); 
} 