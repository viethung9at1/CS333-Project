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