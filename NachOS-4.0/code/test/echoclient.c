#include "syscall.h" 
#include "copyright.h" 

#define NumSocket 4

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
    int sId[NumSocket], i;
    //int PP;
    char content[101];
    //char IP[MAXIpAddressLength+1], Port[7];
    char sms[100];
    //_strcpy(sms, "IP:");Write(sms, 20, _ConsoleOutput);
    //Read(IP, MAXIpAddressLength, _ConsoleInput);
    //_strcpy(sms, "Port:");Write(sms, 20, _ConsoleOutput);
    //Read(Port, 7, _ConsoleInput);

    int PP = 1234;
    char IP[] = "127.0.0.1";

    //_strcpy(sms, IP);Write(sms, 20, _ConsoleOutput);
    
    //_strcpy(sms, "Mess:");Write(sms, 20, _ConsoleOutput);
    // SOCKET
    for (i=0; i<NumSocket; ++i){
        sId[i] = SocketTCP();
        //PP = string_to_int(Port);
        _strcpy(sms, "Mess:");Write(sms, 20, _ConsoleOutput);
        Read(content, 101, _ConsoleInput);
        
        if (Connect(sId[i], IP, PP) == 0){
            //Send(sId[i], content, 101)
            if (Write(content, 101, sId[i]) > 0){
                //Receive(sId[i], content, 101)
                if (Read(content,101,sId[i]) > 0){
                    _strcpy(sms, "Receive:\n");Write(sms, 20, _ConsoleOutput);
                    Write(content, 101, _ConsoleOutput);
                }else{ 
                    _strcpy(sms, "Receive fail !\n");Write(sms, 20, _ConsoleOutput);
                }
            }else{
                _strcpy(sms, "Send fail !\n");Write(sms, 20, _ConsoleOutput);
            }
        }else{
            _strcpy(sms, "Connect fail !\n");Write(sms, 20, _ConsoleOutput);
        }
    }
    // CLOSE
    for (i=0; i<NumSocket; ++i)
        Close(sId[i]);

    Halt(); 
} 
