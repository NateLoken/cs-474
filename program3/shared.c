// Nate Loken
//CS-474 Operating Systems
// The purpose of this program is to perform mergesort using children processes to handle parts of the work to speed it up.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define MAXNUM 100000
int *in;
int out[MAXNUM];

// Simple helper functions
void printArray(int length)
{
   for(int i = 0; i < length; i++)
      printf("%d ", in[i]);
   printf("\n");
}

void isSorted(int length)
{
    for(int i = 1; i < length; i++)
        if(in[i-1] > in[i])
        {
            printf("Array is not sorted\n");
            return;
        }
    printf("Array is sorted\n");
}

// Merge function from original code, modified to use the shared memory segment
void merge(int start, int mid, int end)
{
   int i, j, k;

   i = start;
   j = mid + 1;
   k = start;

   while((i <= mid) && (j <= end))
   {
      if(in[i] > in[j])
      {
         out[k] = in[j];
         k += 1;
         j += 1;
      }
      else
      {
         out[k] = in[i];
         k += 1;
         i += 1;
      }
   }

   while(i <= mid)
   {
      out[k] = in[i];
      k += 1;
      i += 1;
   }
   
   while(j <= end)
   {
      out[k] = in[j];
      k += 1;
      j += 1;
   }

   for(i = start; i <= end; i++)
      in[i] = out[i];
}

void sort(int start, int end)
{
   int mid;
   if(start < end)
   {
      mid = (start + end) / 2;

      sort(start, mid);
      sort(mid + 1, end);

      merge(start, mid, end);
   }
}

/*
   childSort takes the start and end values to determine how to split the array, then the level value to determine when to stop creating children
   then teh share_key and item values to link the shared memory segment to each child.

   The function recusively creates a child to do one half of the merge sort while the parent does the other half util 8 procresses(including the parent) are working on the
   sort at once.
*/
void childSort(int start, int end, int level, int share_key, struct shmid_ds item){
   pid_t child;
   int status;
   int mid;
   mid = (start + end) / 2;
   if(level == 0){
      sort(start, end);
   }
   else{
      child = fork();
      if(child == 0){
         if((in = shmat(share_key, (void *)0, SHM_RND)) == (void *) -1)
            perror("Cannot attach memory\n");
         childSort(start, mid, level - 1, share_key, item);
         exit(EXIT_SUCCESS);
      }
      else{
         if((in = shmat(share_key, (void *)0, SHM_RND)) == (void *) -1)
            perror("Cannot attach memory\n");
         childSort(mid + 1, end, level - 1, share_key, item);
         waitpid(child, &status, 0);
         merge(start, mid, end);

         shmctl(share_key, IPC_RMID, &item);
      }
   }
}

int main(int argc, char* argv[])
{
   int size, i;
   int start, end;
   int share_key_in;
   struct shmid_ds item;

   long long j, k;
   struct timespec s_startTime, s_endTime;

   // creating shared memory segment
   if((share_key_in = shmget(IPC_PRIVATE, MAXNUM * 4, IPC_CREAT|0666)) < 0)
      perror("Cannot get shared memory\n");
   
   // linking segment to in variable
   if((in = shmat(share_key_in, (void *)0, SHM_RND)) == (void *) -1)
      perror("Cannot attach to shared memory");

   i = 0;

   // Getting array size from command line args
   size = atoi(argv[1]);

   // Generating array
   for(i; i < size; i++)
      in[i] = rand() % 1000;

   // if procress doesn't have a large enough array to take advantage of multiprocessing then do sort normally
   if(i <= 8)
   {
      if(clock_gettime(CLOCK_REALTIME, &s_startTime) != 0)
      {
         perror("clock_gettime error\n");
         exit(EXIT_FAILURE);
      }
      sort(0, size-1);
      if(clock_gettime(CLOCK_REALTIME, &s_endTime) != 0)
      {
         perror("clock_gettime error\n");
         exit(EXIT_FAILURE);
      }

      j = s_endTime.tv_sec - s_startTime.tv_sec;
      k = s_endTime.tv_nsec - s_startTime.tv_nsec;
      j = j * 1000000000 + k;

      isSorted(size);
      printf("Execution Time: %lld ns\n", j);
      shmctl(share_key_in, IPC_RMID, &item);
   }
   else
   {
      start = 0;
      end = size - 1;

      printArray(size);
      if(clock_gettime(CLOCK_REALTIME, &s_startTime) != 0)
      {
         perror("clock_gettime error\n");
         exit(EXIT_FAILURE);
      }
      
      childSort(start, end, 3, share_key_in, item);
      
      if(clock_gettime(CLOCK_REALTIME, &s_endTime) != 0)
      {
         perror("clock_gettime error\n");
         exit(EXIT_FAILURE);
      }
      printArray(size);

      j = s_endTime.tv_sec - s_startTime.tv_sec;
      k = s_endTime.tv_nsec - s_startTime.tv_nsec;
      j = j*1000000000 + k;
      
      isSorted(size);
      printf("Execution Time: %lld ns\n", j);
   }
}