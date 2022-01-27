/*
 * Nate Loken
 * 9/12/2021
 * The purpose of this file is to facilitate the execution of commands coming from the main shell.c loop
 */
#include <fcntl.h>
#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

// Simple helper program to print the present working directory
void printDir()
{
  char cdir[200];
  if(getcwd(cdir, 200) != NULL)
    printf("%s\n", cdir);
  else
    perror("pwd failed");
}

/*
 * Execute:
 *  Takes 2 parameters, the number of arguments passed and a list of character strings that represent the argument
 *  
 *  First execute checks if the argument matches any of the built in functions, these include:
 *    echo
 *    pwd
 *    cd
 *    mkdir
 *    exit
 *  If the argument doesn't match one of those then it is then passed off to the child process handler at the end of the if else chain
 *  This handler first checks for the file IO redirection arguments '<' and '>' and redirects the output accordingly.
 *  Next the child process is created and execv is used to inject the executable code using the path given, execv also takes into account arguments.
 *
 *  The return value of execute is either 1, or 0 where 1 will continue and 0 will exit out of the shell loop.
 */

int execute(int args, char* argv[])
{
  int fp_in, fp_out;
  int status;
  pid_t child;



  if(strcmp(argv[0], "echo") == 0)
  {
    for(int i = 1; i <= args; i++)
    {
      printf("%s ", argv[i]);
    }
    printf("\n");
  }

  else if(strcmp(argv[0], "pwd") == 0)
  {
    printDir();
  }

  else if(strcmp(argv[0], "cd") == 0)
  {
    if(chdir(argv[1]) != 0)
        perror("cd failed");
    printDir();
    
  } 

  else if(strcmp(argv[0], "mkdir") == 0)
  {
    for(int i = 1; i <= args; i++)
    {
      if(mkdir(argv[i], 0777) != 0)
        perror("mkdir failed");
    }
  }

  else if(strcmp(argv[0], "exit") == 0)
  {
    return 0;
  }

  else
  {

    child = fork(); 

    // Parent
    if(child != 0)
      waitpid(child, &status, 0);

    // Child
    else
    {
      /* Parse through arguments and if redirection argument is found then file attempts to open and IO is redirected using dup2
       * and argument is set to NULL essentially removing it from the argument list*/
      for(int i = 1; i <=  args; i++)                                      
      {
        if(strcmp(argv[i], "<") == 0)
        {
          if((i + 2) <= args)
            if(!(strcmp(argv[i+2], ">") == 0))
            {
              fprintf(stderr, "Incorrect usage of \'<\'\n");
              exit(EXIT_FAILURE);
            }
        
          fp_in = open(argv[i+1], O_RDONLY);
          argv[i] = NULL;
          if(fp_in == -1)
            perror("Opening File");

          if(dup2(fp_in, STDIN_FILENO) == -1)
            perror("Redirecting STDIN");
        }//end if
        
       else if(strcmp(argv[i], ">") == 0)
        {
          if((i + 2) <= args)
          {
            fprintf(stderr, "Incorrect usage of \'>\'\n");
            exit(EXIT_FAILURE);
          }
            
          fp_out = open(argv[i+1], O_RDWR|O_CREAT|O_APPEND|O_CLOEXEC, 0600);
          argv[i] = NULL;
          if(fp_out == -1)
          perror("Opening File");
          
          if(dup2(fp_out, STDOUT_FILENO) == -1)
          perror("Redirecting Output");
        }//end else if 
      }//end for

      // Executes command
      if(execv(argv[0], argv) < 0)
      {
        fprintf(stderr, "%s: Command not found\n", argv[0]);
        errno = 1;
      }
      exit(EXIT_SUCCESS); 
    }
  }
  return 1;
}
