# Report
Project 1  
Course: Introduction to Operating System - CS333  
## Team member  
20125058 - Nguyen Viet Hung  
20125010 - Tran Bao Loi  
20125027 - Phan Minh Duy
# Content
## File section  
### Create file  
In this section, I follow the instruction that the instructor give. Just follow this and change the **machine** to **kernel->machine** to match the version (the instruction was writen with NachOS 3.4).  
I add the **SC_Create** system call, read data from user by reading *r4* register, trasfer it from user space to system space. If file name is null, throw and exception. Else, create the file using **fileSystem->Create(string filename)**
### Open, close file
#### Open file
To open the file, we have **SC_Open** system call. Similar to the section above, we get  user input data from register *r4*, *r5*. After that, we move the data from user space to kernel space. We have maximum opening file is 20, so that I create the *openingFile* array with 20 elements. If there is any free slot in the *openingFile* array, We use the **fileSystem->Open** to open the file and assign this file to the *openingFile* array. We also assign the file type in this section.  
#### Close file  
To close the file,we have system call **SC_Close** we delete the file instance in *openingFile* array.  
### Read, write file
#### Read 
We have system call **SC_Read** to do this work. Firstly, we check that if the file is opened. If we open the *stdin* file, then we read data from user. Else if we open the *stdout* file, we print that the *stdout* file can't be read. For opening the normal file, we read the file content and print it on the console. We return the number of charactors are read.
#### Write  
We have system call **SC_Write** to do this work. Firstly, we check that if the file is opened. If we open the *stdout* file, then we print data to the console. Else if we open the *stdin* or read-only file, we print that the *stdin* or read-only file can't be read. For opening the normal file, we write data from the console to the file content. We return the number of charactors are write.
### Seek  
To seek the file, we use the system call **SC_Seek**. This system call use the build in *LSeek* function in *sysdep.h*. Before seeking one file, it check if the file is being opened. It throw exception if the file is not existed or the file is *stdin* or *stdout*, these file can't be seeked 
### Remove  
We use **SC_Remove** to remove the file. To remove the file, we check if the file is opening. If the file is opening, the system throw an exception because that the opening file can't be deleted. Else, the file is deleted using NachOS's built-in function *fileSystem->Remove*

## Implement system call for Network operations
### Initialize socket TCP method:
First, I will define some constants: reverseFD = 2 and MaxFileOpen = 20 in the file “filesys.h”. 
This function generates a TCP socket by identifying an available slot in an array named fileSocket. The creation of the fileSocket array requires the definition of a user-friendly openFileSocket struct that manages all files associated with this section. For additional information, refer to the “filesys.h” file.

After checking there is a free slot or not by using the function checkSlotSocket() in the “filesys.h”, we will call the function openSocketInternet(), which is initialized in the file sysdep.cc. This function opens a socket for an internet connection using the TCP protocol. It first creates a socket by calling the socket function with the address family set to AF_INET for IPv4, the socket type set to SOCK_STREAM for a reliable stream-oriented connection, and the protocol set to 0 to use the default protocol for the specified address family and socket type.

Next, I'll proceed with the createTCP() function, found in the "filesys.h" file. As mentioned earlier, I'll first search for an available slot, referred to as parameter "a". If such a slot is identified, I will invoke the openSocketInternet() function and pass parameter "a" to the corresponding position within the fileSocket array. Ultimately, the function will return the index of position "a".

Finally, we will process the function createTCP() in the file “exception.cc”.  The openSystemSocket() function is responsible for creating a socket for the system. It first calls the createTCP() function to obtain a socket ID. If the socket ID is -1, indicating an error, it prints an error message and writes -1 to register 2. If the socket is successfully created, it prints a success message along with the socket ID and writes the socket ID to register 2. Finally, the program counter is incremented in both cases.

### Connect socket TCP method:
For this function, I will reuse all of the function which is mentioned in the method of creating socket TCP. This function attempts to establish a TCP connection with the specified IP address and port number. It first checks if the socket ID is within the valid range and if the socket at the given socket ID exists. If both conditions are met, the function calls connectTCP() with the fileSocket, IP address, and port number as arguments and returns the result.
Next, I will continue to the function connectSystemSocket() in the file “exception.cc”. This function can be described as that: 
-	Print a debug message indicating the function has been called.
-	Read the socket ID from register 4.
-	Read the virtual address from register 5.
-	Read the IP address from the given virtual address (using the User2System() function) and store it in the ip variable.
-	Check if the IP address is NULL; if so, write -1 to register 2, delete the IP, increase the program counter (PC), and return from the function.
-	Read the port from register 6.
-	Attempt to connect the TCP socket using the connectTCP() function with the given socket ID, IP address, and port.
-	If the connection fails, print an error message, write -1 to register 2, delete the IP, increase the PC, and return from the function.
-	If the connection is successful, print a success message with the socket ID, IP address, and port.
-	Write 0 to register 2, delete the IP, and increase the PC.
Finally, I will handle this case in function ExceptionHandler().

### Send socket TCP method:
The sendTCP() function sends data over a TCP connection using a given socket identifier, data buffer, and port. It returns -1 for invalid sockets, 0 if the socket is not open or initialized, and otherwise calls another sendTCP function with the associated file descriptor to perform the actual data transfer.
Next, I will continue to the function sendSystemSocket(), which is defined in the file “exception.cc”. We can illustrate this function as that:
-	Reads the socket ID from a register.
-	Reads the virtual address of the data buffer from a register.
-	Translates the virtual address to a system address, and copies the buffer content.
-	Checks if there's enough memory, and if not, it writes an error value to a register, deallocates the buffer, and increases the program counter before returning.
-	Reads the data length from a register.
-	Sends the data using the sendTCP function of the file system, and stores the return value.
-	Prints the result of the data transfer (failed, connection closed, or successful).
-	Writes the return value to a register, deallocates the buffer, and increases the program counter.
-	The function mainly deals with reading values from registers, managing memory, and handling the TCP data transfer within the kernel.

### Receive socket TCP method:
We will demonstrate how two function, which is related to this question work:
-	receiveTCP() in “filesys.h”: This function is used to receive data from a TCP socket. It takes three arguments: socketid, buffer, and port. The function checks if the provided socketid is valid, and whether the associated file socket exists. If the conditions are met, it calls another version of receiveTCP with the file descriptor, buffer, and port as arguments. The function returns -1 if there's an error, 0 if the file socket is null, or the result of the nested receiveTCP call.
-	receiveSystemSocket() in “exception.cc”: This function is used to handle a system call for receiving data over a TCP socket. It reads the socket ID, virtual address, and length from the machine registers, and then allocates a buffer of the specified length. It then calls the receiveTCP function from the file system using the provided socketID, buffer, and length. Depending on the return value of receiveTCP, the function displays an appropriate message (failed to send, connection closed, or successfully received data) and writes the received data to the user space if applicable. Finally, it updates the return value in the machine register and increases the program counter.

### Close socket TCP method:
-	int closeTCP(int socketid): This function takes an integer socketid as input, representing the ID of a socket to be closed. It checks whether the given socket ID is within valid range (between reverseFD and MaxFile). If it's not in range or if the socket is not open (i.e., fileSocket[socketid] is nullptr), the function returns -1, indicating an error. Otherwise, it proceeds to close the socket by deleting the corresponding fileSocket entry and setting it to nullptr. Finally, it returns 0, indicating a successful close operation.

-	void systemCloseSocket(): This function is a higher-level wrapper for the closeTCP() function. It first reads the socket ID from register 4 and stores it in socketID. It then calls the closeTCP() function with the obtained socket ID and stores the return value in rVal. The return value (0 for success, -1 for error) is then written to register 2. Finally, it calls PCIncrease() to increment the program counter, allowing the program to proceed to the next instruction.



