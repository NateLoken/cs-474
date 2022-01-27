//Nate Loken
//8/24/2021
//Simple signal handler for SIGUSR1 and SIGUSR2

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int signum)
{
  // Printing the exam date and time if the signal is SIGUSR1 
  if(signum == SIGUSR1)
    printf("The final exam is December 9,2021 at 8:00am\n");
  // Prints if I have any conflicting times if the signal is SIGUSR2
  else if(signum == SIGUSR2)
    printf("Nathaniel has no conflict with the exam time\n");
  else
   fprintf(stderr, "recieved signal %d\n", signum);
}

int main()
{
  // Registering the signal handler for SIGUSR1 and SIGUSR2
  // If this either fail then prints to the console and returns 1
  if(signal(SIGUSR1, sig_handler) == SIG_ERR)
  {
    printf("\ncan't catch SIGUSR1\n");
    return 1;
  }
  if(signal(SIGUSR2, sig_handler) == SIG_ERR)
  {
    printf("\ncan't catch SIGKILL\n");
    return 1;
  }

  while(1)
  {
    sleep(1);
  }
  return 0;
}
