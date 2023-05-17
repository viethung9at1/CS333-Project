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

    fileId = Open("info.txt", 1);
    if (fileId == -1){
        _strcpy(sms, "fail\n");Write(sms, MaxFileLength, _ConsoleOutput);
    }else{
        Wait("lock");        
        len = getlengthoffile(fileId);
        Read(content, len, fileId);
        Write(content, len, _ConsoleOutput);
        Close(fileId);
        Signal("lock");
    }
    Exit(0);
} 