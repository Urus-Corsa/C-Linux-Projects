#include<unistd.h>
#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAX_NUM_LINES 1024
#define MAX_LINE_LENGTH 5

// TRACE_NODE_STRUCT is a linked list of
// pointers to function identifiers
// TRACE_TOP is the head of the list is the top of the stack

struct TRACE_NODE_STRUCT
{
    char* functionid; // ptr to function identifier
    struct TRACE_NODE_STRUCT* next; // ptr to next frama
};

struct LINK_NODE
{
    char* line; // ptr to function identifier
    int index;
    struct LINK_NODE* next; // ptr to next frama
};

typedef struct TRACE_NODE_STRUCT TRACE_NODE;
typedef struct LINK_NODE NODE;

static TRACE_NODE* TRACE_TOP = NULL; // ptr to the top of the stack

NODE *head=NULL; //initialize the linked list.

/* --------------------------------*/
/* function PUSH_TRACE */

void PUSH_TRACE(char* p) // push p on the stack
{
    TRACE_NODE* tnode;
    static char glob[]="global";

    if (TRACE_TOP==NULL) {
// initializes the stack with "global" identifier
        TRACE_TOP=(TRACE_NODE*) malloc(sizeof(TRACE_NODE));
// no recovery needed if allocation failed, this is only
// used in debugging, not in production

        if (TRACE_TOP==NULL) {
            printf("PUSH_TRACE: memory allocation error\n");
            exit(1);
        }

        TRACE_TOP->functionid = glob;
        TRACE_TOP->next=NULL;

    }

// create the node for p
    tnode = (TRACE_NODE*) malloc(sizeof(TRACE_NODE));
// no recovery needed if allocation failed, this is only
// used in debugging, not in production

    if (tnode==NULL) {
        printf("PUSH_TRACE: memory allocation error\n");
        exit(1);
    }

    tnode->functionid=p;
    tnode->next = TRACE_TOP; // insert fnode as the first in the list

    TRACE_TOP=tnode; // point TRACE_TOP to the first node
}/*end PUSH_TRACE*/

/* --------------------------------*/

/* function POP_TRACE */
void POP_TRACE() // remove the op of the stack
{
    TRACE_NODE* tnode;
    tnode = TRACE_TOP;
    TRACE_TOP = tnode->next;
    free(tnode);
} //end POP_TRACE


/* function PRINT_TRACE prints out the sequence of function calls that are on the
stack at this instance */

char* PRINT_TRACE()
{
    int depth = 50; //A max of 50 levels in the stack will be combined in a string for printing out.
    int i, length, j;
    TRACE_NODE* tnode;
    static char buf[100];

    if (TRACE_TOP==NULL)
    { // stack not initialized yet, so we are
        strcpy(buf,"global"); // still in the `global' area
        return buf;
    }

    sprintf(buf,"%s",TRACE_TOP->functionid);

    length = strlen(buf); // length of the string so far

    for(i=1, tnode=TRACE_TOP->next; tnode!=NULL && i < depth; i++,tnode=tnode->next)
    {
        j = strlen(tnode->functionid); // length of what we want to add
        if (length+j+1 < 100)
        { // total length is ok
            sprintf(buf+length,":%s",tnode->functionid);
            length += j+1;
        }
        else // it would be too long
        {
            break;
        }
    }
    return buf;
}

// function REALLOC calls realloc
// REALLOC also prints info about memory usage.

void* REALLOC(void* p,int t,char* file,int line, const char *function )
{
    printf("File %s, line %d, function %s reallocated the memory at address %p to a new size %d\n", file, line, function, p, t);
    p = realloc(p,t);

    printf("FUNCTION STACK TRACE: %s\n", PRINT_TRACE());
    return p;
    POP_TRACE();

}

// function MALLOC calls malloc
// MALLOC also prints info about memory usage.

void* MALLOC(int t,char* file,int line,const char *function)
{
    void* p;
    p = malloc(t);
    printf("File %s, line %d, function %s allocated new memory segment at address %p to size %d\n", file, line, function, p, t);
    printf("FUNCTION STACK TRACE: %s\n", PRINT_TRACE());
    return p;
    POP_TRACE();
}

// function FREE calls free
// FREE also prints info about memory usage.
// For instance, example of print out:

void FREE(void* p,char* file,int line, const char *function)
{
    printf("File %s, line %d, function %s deallocated the memory segment at address %p\n", file, line, function, p);
    free(p);
    printf("FUNCTION STACK TRACE: %s\n", PRINT_TRACE());
    POP_TRACE();
}

void addToList(char *line, int index)
{
    NODE *temp = (NODE *)malloc(sizeof(NODE));
    temp->line = (char *)malloc(strlen(line)+1);

    memset(temp->line, '\0', strlen(line)+1);

    strncpy(temp->line, line, strlen(line)+1);
    temp->index=index;
    temp->next=NULL;

    if (head == NULL)
    {
        head=temp;
    }
    else
    {
        NODE *t=head;
        while(t->next != NULL)
        {
            t=t->next;
        }
        t->next=temp;
    }
}

void printList()
{
    NODE *t=head;
    printf("Following are the content of linked list: \n");
    while(t)
    {
        printf("Index: %d Line: %s", t->index, t->line);
        t=t->next;
    }
    printf("\n");
}

void freeList()
{
    NODE *t=head;
    while(t)
    {
        NODE *temp=t;
        t=t->next;
        free(temp);
    }
}

#define realloc(a,b) REALLOC(a,b,__FILE__,__LINE__,__FUNCTION__)
#define malloc(a) MALLOC(a,__FILE__,__LINE__,__FUNCTION__)
#define free(a) FREE(a,__FILE__,__LINE__,__FUNCTION__)

int main(int argc, char **argv)
{
    char **array;
    int i=0, j=0;
    int totalLines=0;
    char lineRead[1024];
    FILE *fptr = NULL;
    int index = 0;

    if ((fptr = fopen("input", "r")) == NULL) {
        printf("Error! opening input file");
        //program exits if file pointer returns NULL.
        exit(1);
    }

    int fDesc = open("memtrace.out",O_WRONLY|O_CREAT|O_TRUNC);

    //not stdout points to file_desc
    dup2(fDesc, 1) ;


    array=malloc(MAX_NUM_LINES*sizeof(char *));
    for(i=0; i<MAX_NUM_LINES; i++)
    {
        array[i]=malloc(MAX_LINE_LENGTH * sizeof(char));
    }


    while((fgets (lineRead, 1024, fptr))!= NULL)
    {
        int len=strlen(lineRead);

        if(index >= MAX_NUM_LINES)
        {
            array=realloc(array, index+1);
            array[index]=malloc(len * sizeof(char));
        }
        else if (len > MAX_LINE_LENGTH)
        {
            array[index] = realloc(array[i], MAX_LINE_LENGTH+len);
        }
        strncpy(array[index], lineRead, len);
        addToList(lineRead, index);
        index++;
    }


    array=realloc(array, ((MAX_NUM_LINES+1)* sizeof(char *)));
    array[MAX_LINE_LENGTH]=malloc(MAX_LINE_LENGTH * sizeof(char));


    close(fDesc);
    fclose(fptr);

    printList();
    freeList();

    printf("Printing from Array: \n");
    for (i=0; i<index; i++)
    {
        printf("%d: %s", i, array[i]);
    }

    for (i=0; i<index; i++)
    {
        free(array[i]);
    }

    free(array);
    return 0;
}
