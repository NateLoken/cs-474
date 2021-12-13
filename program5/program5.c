// Nate Loken
// CS-474 Operating Systems
// The purpose of this program is to simulate rudementary memory management using techniques such as base and bounds, garbage collection using complaction, and contiguous memory tracking.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCCESS 10
#define DELIMS " \t\n\a\r"
int time;

typedef struct process
{
    char name[10];
    int base;
    int active;
    int bounds;
    int sTime;
} process;

typedef struct iNode 
{
    int base;
    int bounds;
    struct iNode *next;
} iNode;

// simple function to add nodes to the linked list
void addNode(struct iNode **head_ref, int base_num, int bound_num)
{
    struct iNode *newNode = malloc(sizeof(struct iNode));
    newNode->base = base_num;
    newNode->bounds = bound_num;
    newNode->next = (*head_ref);

    (*head_ref) = newNode;
}

/*
    Defrag:
        Takes 3 parameters, a reference to the head node, a reference to the process table, and the length of the process table

    Defrag compacts the memory by first moving all the entries in the table to the bottom of the memory stack and then coalates all the free
    memory being stored in the linked list by walking through the list and getting the total available memory, then that is stored as the bounds for the new chunk.
    Next the base is calculated by taking the new base of the final procpess in the table and adding it's bounds to it. finallly the old nodes are destroyed using the free function
*/
void defrag(struct iNode **head_ref, struct process* table, int array_len)
{
    table[0].base = 0;
    for(int i = 1; i < array_len; i++)
    {
        table[i].base = table[i-1].base + table[i-1].bounds;
    }
        
    int total = 0;
    struct iNode *tmp = (*head_ref);
    while((tmp)->next != NULL)
        total += tmp->bounds;

    (*head_ref)->base = table[array_len-1].base;
    (*head_ref)->bounds = total;
    tmp = (*head_ref)->next;
    (*head_ref)->next = NULL;
    printf("Time: %d\n", time);
    free(tmp);
}

/*
    findChunk:
        findChunk takes 3 parameters: the reference to the head of the linked list, the chunk size that is trying to be found, and a reference to the process table and table length
        in case the program needs to defrag.

        This function works by walking though the linked list of memory chunks and checking to see if any are large enough to fit the desired chunk size. If there is an available chunk
        then the base coresponding to that chunk is returned and the chunk node is updated. However, if no one chunk is found then the function must compact the available memory to create
        a large enough chunk

        NOTE: There will never be an instance of findChunk trying to find a chunk when there isn't enough memory to handle the size of a program as the total memory available is always
        tracked.
*/
int findChunk(struct iNode **head_ref, int chunkSize, struct process* table, int array_len)
{
    int base;
    while((*head_ref)->next != NULL && (*head_ref)->bounds < chunkSize)
        (*head_ref) = (*head_ref)->next;

    if((*head_ref)->next == NULL && (*head_ref)->bounds < chunkSize)
    {
        defrag(head_ref, table, array_len);
        return (*head_ref)->base;
    }
    else
    {
        base = (*head_ref)->base;
        (*head_ref)->bounds -= chunkSize;
        (*head_ref)->base += chunkSize;
        return base;
    }
}

// Simple parser to tokenize input
int parse(char* buffer, char* argv[])
{
    int j = -1;
    char* token;

    token = strtok(buffer, DELIMS);

    while(token != NULL)
    {
        j++;
        argv[j] = token;
        token = strtok(NULL, DELIMS);
    }

    argv[j+1] = '\0';
    return j;
}

// Remove element function to be used when a STOP event is created
void remove_element(struct process* table, int index, int array_len)
{
    int i;
    for(i = index; i < array_len - 1; i++) table[i] = table[i + 1];
}

int main()
{
    char *line = NULL;
    char* argv[10];
    ssize_t buffer = 0;
    size_t size;
    int args, time, status, totalMem, pCount;
    struct process* processTable = (process*)malloc(sizeof(struct process) * 1);
    struct iNode *head = NULL;
    struct iNode *freePtr = malloc(sizeof(struct iNode));

    status = 1;
    totalMem = 0;
    pCount = 0;
    size = 1;
    time = 1;

    printf("RAM ");
    getline(&line, &buffer, stdin);
    parse(line, argv);
    addNode(&head, 0, atoi(argv[0]));
    totalMem = atoi(argv[0]);
    freePtr = head;


    while(status)
    {
        printf("time %d: ", time);
        fflush(stdout);

        if(getline(&line, &buffer, stdin) == -1 && !(feof(stdin)))
            perror("Readline");

        args = parse(line, argv);

        if(strcmp(argv[1], "EXIT") == 0 || strcmp(argv[0], "exit") == 0)
        {
            status = 0;
        }
        // The START event creates a new process adds it to the dynamically allocated array, however, if there is no available memory or there are already 10 processes runnign then 
        // an error will be thrown.
        else if(strcmp(argv[1], "START") == 0)
        {
            if(pCount < MAX_PROCCESS && atoi(argv[3]) <= totalMem)
            {
                if(pCount >= size)
                {
                    size += 1;
                    processTable = realloc(processTable, sizeof(struct process) * size);
                    if(!processTable)
                    {
                        fprintf(stderr, "Reallocation error\n");
                        exit(EXIT_FAILURE);
                    }
                }
                struct process newProcess;
                strcpy(newProcess.name, argv[2]);
                newProcess.base = findChunk(&freePtr, atoi(argv[3]), processTable, size);
                newProcess.bounds = atoi(argv[3]);

                if(atoi(argv[0]) == time)
                    newProcess.active = 1;
                else
                    newProcess.active = 0;

                newProcess.sTime = atoi(argv[0]);
                totalMem -= atoi(argv[3]);

                processTable[pCount] = newProcess;
                pCount += 1;
            }
            else
                fprintf(stderr, "Cannot add any more processes\n");
        }
        // the STOP event first finds the process and then creates a new node in the linked list of the memory address and size, and then is removed by removeing it from the list and reallocating
        // the table
        else if(strcmp(argv[1], "STOP") == 0)
        {
            int index;
            for(index = 0; index < pCount; index++)
                if(strcmp(processTable[index].name, argv[2]) == 0)
                    break;
            addNode(&freePtr, processTable[index].base, processTable[index].bounds);
            totalMem += processTable[index].bounds;
            remove_element(processTable, index, size);
            struct process* tmp = realloc(processTable, sizeof(struct process) * (size - 1));
            if(tmp == NULL && size > 1)
            {
                fprintf(stderr, "No more memory available\n");
                exit(EXIT_FAILURE);
            }
            size -= 1;
            pCount -= 1;
            processTable = tmp;
        }
        // The DUMP event simply prints the base and bounds of all active processes by using a for loop to walk through the list and an if statement to skip any inactive processes
        else if(strcmp(argv[1], "DUMP") == 0)
        {
            for(int i = 0; i < pCount; i++)
            {
                if(processTable[i].active == 0)
                    i += 1;
                printf("%s:\n\tBase:%d\n\tBounds:%d\n", processTable[i].name, processTable[i].base, processTable[i].bounds);
            }
        }
        // The LOC event checks that the memory reference made is in the bounds allocated to the process
        else if(strcmp(argv[2], "LOC") == 0)
        {
            int index;
            for(index = 0; index < pCount; index++)
                if(strcmp(processTable[index].name, argv[1]) == 0)
                    break;
            if((processTable[index].base + atoi(argv[3])) > (processTable[index].base + processTable[index].bounds))
                fprintf(stderr, "Cannot reference memory beyond bounds\n");
        }
        time++;
        // loop to find and active any processes that need to be activated at a future time
        for(int i = 0; i < pCount; i++)
            if(processTable[i].sTime == time)
                processTable[i].active = 1;
    }
    free(processTable);
    free(freePtr);
}