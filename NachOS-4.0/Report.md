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
I add the **SC_Create** system call, read data from user by reading *r4* register, trasfer it from user space to system space. If file name is null, throw and exception. Else, create the file using 
