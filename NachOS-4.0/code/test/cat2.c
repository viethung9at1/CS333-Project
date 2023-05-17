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

int main(int argc, char **argv) { 
    int fileId, len;
    char sms[100];
    char* filename;
    char* content;

    if (argc != 2)
        return -1;
    
    fileId = Open(argv[1], 1);
    len = getlengthoffile(fileId);
    Read(content, len, fileId);
    
    Write(content, len, _ConsoleOutput);

    Exit(0);  
} 