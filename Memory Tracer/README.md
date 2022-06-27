Memory Leaks Prevention

One of the main problems in memory management with C is memory leaks. Memory leaks in an Operating System cause serious runtime problems and they can make a system unstable.

The program mem_tracer.c reads all lines from an input file. However, it reads all lines into an array of type char ** .
First, it initializes the array memory with malloc to an initial size. Using realloc, it expands the array if the initial size turns out not to be big enough for the lines in the input file. The program works with an input file of up to a few hundred lines.
Then, it stores each line in a linked list where a node contains the line as a string (char *) and index number. Additionally, the recursive function PrintNodes prints out the content of all nodes in the linked list.
Using a stack holding the function names and printing the memory consumed whenever memory is allocated, reallocated or freed, the memory consumption gets traced accordingly. The tracing prints messages describing the memory management and usage in each function.
The allocation and deallocation for the char ** array and your linked list should make use of your memory tracing in order to print messages describing the memory management and usage.
The parent process redirects the stdout to a log file named "memtrace.out". The dup2() call will replace the file descriptor STDOUT_FILENO with the opened file for memtrace.out.

In conclusion, the program ensures that there are no memory leaks. 

Copy rights reserved.
