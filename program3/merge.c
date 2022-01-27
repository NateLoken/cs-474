/*
    Nate Loken
    CS-474 Operating Systems

    The purpose of this code is to sort an array of size n, where n is defined by the user, using the merge sort algorithm.
    this program doesn't spawn additional processes or threads.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


// Simple function to print array
void printArray(int arr[], int length)
{
    for(int i = 0; i < length; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

// Helper function to determine if array is sorted without looking at it
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

/*
    Merge takes an array a start, middle, and end value
    then compares each value to see if they need to be swapped and adds each value to a temporary array,
    then it merges the sorted part of the temporary array into the original.
*/
void merge(int array[], int start, int middle, int end)
{
    int* temp = malloc(sizeof(int) * (end - start + 1));

    int i = start, j = middle + 1, k = 0;
    while((i <= middle) && (j <= end))
    {
        if(array[i] > array[j])
        {
            temp[k] = array[j];
            k += 1;
            j += 1;
        }
        else
        {
            temp[k] = array[i];
            k += 1;
            i += 1;
        }
    }

    while(i <= middle)
    {
        temp[k] = array[i];
        k += 1;
        i += 1;
    }

    while(j <= end)
    {
        temp[k] = array[j];
        k += 1;
        j += 1;
    }

    for(i = start; i <= end; i++)
        array[i] = temp[i - start];
}

/*
    Mergesort takes and array and start and end value where it calculates the middle value to determine how to split the array
    they recursively calls itself to get the simplest version of each half, at which point it merges each peice back together
    using the merge function.
*/
void mergeSort(int array[], int start, int end)
{
    if(start < end)
    {
        int middle = (start + end) / 2;
        mergeSort(array, start, middle);
        mergeSort(array, middle + 1, end);

        merge(array, start, middle, end);
    }
    else return;
}

int main(int argc, char* argv[])
{
    int size;
    long long j, k;
    struct timespec s_startTimer, s_endTimer;

    // getting array size from command line
    size = atoi(argv[1]);

    // building array
    int* randArray = malloc(sizeof(int) * size);
    for(int i = 0; i < size; i++)
        randArray[i] = rand() % 1000;

    printArray(randArray, size);
    
    // Measuring start time
    if(clock_gettime(CLOCK_REALTIME, &s_startTimer) != 0)
    {
        perror("clock_gettime error\n");
        exit(EXIT_FAILURE);
    }
    mergeSort(randArray, 0, size - 1);

    // Measuring end time
    if(clock_gettime(CLOCK_REALTIME, &s_endTimer) != 0)
    {
        perror("clock_gettime error\n");
        exit(EXIT_FAILURE);
    }

    // Calculating execution time
    j = s_endTimer.tv_sec - s_startTimer.tv_sec;
    k = s_endTimer.tv_nsec - s_startTimer.tv_nsec;
    j = j*1000000000 + k;

    printArray(randArray, size);

    isSorted(randArray, size);
    printf("Execution time: %lld ns\n", j);
}