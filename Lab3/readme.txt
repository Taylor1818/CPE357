


WEEK 3 - In Class Activity

1) 

It is important to use sizeof() operator because the data that you pass in 
May not be the size that you think that it is. Depending on how the data is 
Handled will determine if the space being taken is for instance 4 or 8.

Ex: *pointer could be 4 bytes or it could be 8 depending on how internally it is handled

2)

getChar() returns an int if failed because if it were to return some sort of char
You might not know that it failed. If it returns a value that is not to be expected 
Then it makes it more obvious when expecting a char but getting an int when testing.
If only returned characters then there wouldn't be any value left it could send to
Indicate a failure.

3)

 int *p;

 while( 1 ){
 
 p++;
 printf("%d", p);

 }


This would be a segfault because at some point the pointer will reach a place that 
It does not have access to.

4)

Malloc works by allocating virtual memory depending on the size of data that is given onto 
A heap. It creates a header that is information that will be used by the function itself.
It then accesses the memory based on the break pointer. If the memory is no longer needed
It is then freed to make room for more data to be allocated. 
The difference between library and system calls is that library functions should work on any
Computer that implements the library correctly. Where as the system calls are dependent on 
What os or system you are running. Accessing the process or accessing the library. 
