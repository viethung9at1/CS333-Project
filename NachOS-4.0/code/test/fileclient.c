#include "syscall.h" 
#include "copyright.h" 

int getlengthoffile(int file){
    int len;
    len = Seek(-1, file);
    Seek(0, file);
    return len;
}

int string_to_int(char* str) {
    int result = 0;
    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    }
    while (*str != '\0') {
        if (*str < '0' || *str > '9') {
            break;
        }
        result = result * 10 + (*str - '0');
        str++;
    }
    return result * sign;
}

void _strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}


int main() { 
    int sId, i, Id1, Id2, len;
    //int PP;
    char content[101];
    //char IP[MAXIpAddressLength+1], Port[7];
    char fileA[MaxFileLength+1], fileB[MaxFileLength+1];
    char sms[100];
    //_strcpy(sms, "IP:");Write(sms, 20, _ConsoleOutput);
    //Read(IP, MAXIpAddressLength, _ConsoleInput);
    //_strcpy(sms, "Port:");Write(sms, 20, _ConsoleOutput);
    //Read(Port, 7, _ConsoleInput);

    int PP = 1234;
    char IP[] = "127.0.0.1";

    _strcpy(sms, "file source:");Write(sms, 30, _ConsoleOutput);
    Read(fileA, MaxFileLength, _ConsoleInput);
    _strcpy(sms, "file dest:");Write(sms, 30, _ConsoleOutput);
    Read(fileB, MaxFileLength, _ConsoleInput);
    
    Id1 = Open(fileA, 1);
    Id2 = Open(fileB, _WriteNEW);

    len = getlengthoffile(Id1);

    Read(content, len, Id1);

    // SOCKET
    sId = SocketTCP();
    //PP = string_to_int(Port);
    
    if (Connect(sId, IP, PP) == 0){
        if (Write(content, 101, sId) > 0){
            if (Read(content,101,sId) > 0){
                _strcpy(sms, "Receive success !");Write(sms, 20, _ConsoleOutput);
                Write(content, len, Id2);
            }else{ 
                _strcpy(sms, "Receive fail !");Write(sms, 20, _ConsoleOutput);
            }
        }else{
            _strcpy(sms, "Send fail !");Write(sms, 20, _ConsoleOutput);
        }
    }else{
        _strcpy(sms, "Connect fail !");Write(sms, 20, _ConsoleOutput);
    }

    Close(Id1);
    Close(Id2);
    Close(sId);

    Halt(); 
} 
