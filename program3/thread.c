// Nate Loken
//CS-474 Operating Systems
// The purpose of this program is to perfom mergesort using multiple threads of execution to speed the process up.

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

// Creating type to be able to pass start, end, and array to the functions
typedef struct input
{
    int start;
    int end;
    int* array;
} input;


// Helper fucntions
void printArray(int arr[], int size)
{
    for(int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

void isSorted(int arr[], int length)
{
    for(int i = 1; i < length; i++)
        if(arr[i-1] > arr[i])
        {
            printf("Array is not sorted\n");
            return;
        }
    printf("Array is sorted\n");
}

// Modified merge to use 'input' type but functionally the same
void merge(input* vals)
{
    int i, j, k;
    int middle;
    int* temp = malloc(sizeof(int) * ((*vals).end - (*vals).start) + 2);
    middle = ((*vals).start + (*vals).end) / 2;

    i = (*vals).start;
    j = middle + 1;
    k = 0;

    while(i <= middle && j <= (*vals).end)
    {
        if((*vals).array[i] > (*vals).array[j])
        {
            temp[k] = (*vals).array[j];
            k += 1;
            j += 1;
        }
        else
        {
            temp[k] = (*vals).array[i];
            k += 1;
            i +=1;
        }
    }

    while(i <= middle)
    {
        temp[k] = (*vals).array[i];
        k += 1;
        i += 1;
    }

    while(j <= (*vals).end)
    {
        temp[k] = (*vals).array[j];
        k += 1;
        j += 1;
    }

    for(i = (*vals).start; i <= (*vals).end; i++)
        (*vals).array[i] = temp[i - (*vals).start];
    free(temp);
}

void sort(input* vals)
{
    int temp = (*vals).end;
    if((*vals).start < (*vals).end)
    {
        int middle = ((*vals).start + (*vals).end) / 2;
        (*vals).end = middle;
        sort(vals);
        (*vals).end = temp;
        temp = (*vals).start;
        (*vals).start = middle + 1;
        sort(vals);

        (*vals).start = temp;

        merge(vals);
    }
    else return;
}

/*
    threadedSort takes a single input pointer in to get the start, stop, and array information

    The function starts by initializing each input to the correct values, then inputs those values into the created thread
    when spawning 7 of them to each work on 7/8 of the array while the main thread does the last 1/8 of the sort
*/
void threadedSort(input* in)
{
    pthread_t child[7];
    int status[7];
    input vals[8];
    int seventh;
    seventh = ((*in).start + (*in).end) / 7;

    // Initialiing input values
    vals[0].start = (*in).start;
    vals[0].end = seventh;
    vals[0].array = (*in).array;

    for(int i = 1; i < 7; i++)
    {
        vals[i].start = vals[i-1].end + 1;
        vals[i].end = seventh * (i+1);
        vals[i].array = (*in).array;
    }

    // spawning threads
    for(int i = 0; i < 7; i++)
    {
        if(pthread_create(&child[i], NULL, (void *)sort, &vals[i]) != 0)
        {
            perror("Cannot create thread\n");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < 7; i++)
    {
        pthread_join(child[i], (void **)&status[i]);
    }

    vals[7].start = vals[6].end+1;
    vals[7].end = (*in).end;
    vals[7].array = (*in).array;

    sort(in);

}


int main(int argc, char *argv[])
{
    long long j, k;
    struct timespec s_startTime, s_endTime;
    int size;
    input in;

    // Getting array size from the command line args
    size = atoi(argv[1]);

    int* randArray = malloc(sizeof(int) * size);

    // Initializing array
    for(int i = 0; i < size; i++)
        randArray[i] = rand() % 1000;


    in.start = 0;
    in.end = size - 1;
    in.array = randArray;

    if((size - 1) <= 8)
    {
        if(clock_gettime(CLOCK_REALTIME, &s_startTime) != 0)
        {
            perror("clock_gettime error\n");
            exit(EXIT_FAILURE);
        }
        sort(&in);
        if(clock_gettime(CLOCK_REALTIME, &s_endTime) != 0)
        {
            perror("clock_gettime error\n");
            exit(EXIT_FAILURE);
        }
        j = s_endTime.tv_sec - s_startTime.tv_sec;
        k = s_endTime.tv_nsec - s_startTime.tv_nsec;
        j = j * 1000000000 + k;

        isSorted(randArray, size);
        printf("Execution Time: %lld ns\n", j);
    }
    else
    {
        printArray(randArray, size);
        if(clock_gettime(CLOCK_REALTIME, &s_startTime) != 0)
        {
            perror("clock_gettime error\n");
            exit(EXIT_FAILURE);
        }
    
        threadedSort(&in);
    
        if(clock_gettime(CLOCK_REALTIME, &s_endTime) != 0)
        {
            perror("clock_gettime error\n");
            exit(EXIT_FAILURE);
        }
    
        j = s_endTime.tv_sec - s_startTime.tv_sec;
        k = s_endTime.tv_nsec - s_startTime.tv_nsec;
        j = j*1000000000 + k;

        printArray(randArray, size);

        isSorted(randArray, size);
        printf("Execution Time: %lld ns\n", j);
    }
}
