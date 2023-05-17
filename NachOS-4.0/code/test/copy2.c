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
    int Id1, len, Id2;
    char fileA[MaxFileLength+1], fileB[MaxFileLength+1], sms[100];
    char* content;

    if (argc != 3)
        return -1;
    
    Id1 = Open(argv[1], 1);
    Id2 = Open(argv[2], _WriteNEW);

    len = getlengthoffile(Id1);

    Read(content, len, Id1);
    Write(content, len, Id2);


    Close(Id1);
    Close(Id2);

    Exit(0);
} 