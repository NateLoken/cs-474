//Nate Loken
//9/9/2021
//CS-474
//Program 2
//The purpose of this program is create a simple shell that can execute linux commands such as "cd", "pwd", "echo", etc.
//as well as input ouput redirection

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

/*
 * Function: parse
 * 
 * Returns the number of arguments given by the user
 *
 * buff: NULL terminated character string of arguments to be tokenized
 * argv: NULL terminated List of charcter pointers where the tokenized arguments will be held
 *
 * This function uses strtok to tokenize the characters from buff, and a while loop parse buff and the tokenized 
 * arguments are saved in argv[j]
 */

#define DELIMS " \t\n\a\r" // const variable used to quickly find the delimters serperating the argument tokens
int parse(char* buff, char* argv[])
{
  int j = -1;
  char *point;

 point = strtok(buff, DELIMS);
 // Parse buff for tokens and save them to argv[j]
 while(point != NULL)
 {
   j++;
   argv[j] = point;
   point = strtok(NULL, DELIMS);
 }

 argv[j+1] = '\0';
 return j;
}

/*
 * The main function faciliates the looping of the shell, calling getline to dynamically create a null terminated string that is then passed to parse
 * which tokenizes the arguments given, and then passed to execute which executes the tokenized list.
 *
 * NOTE: Because parse instantiates j to -1, if no argument is passed this then creates causes argv[0] to become the null termination character
 * and causes a segmentation fault as the strcmp() function used in execute cannont compare the NULL value to a string.
 */

int main()
{
  char* line = NULL;
  ssize_t bufsize = 0;
  char* argv[100];
  int status = 1;

  while(status)
  {
    int args;
    // Print the prompt
    printf("prog2~ ");
    fflush(stdout);

    // Read in the line
    if(getline(&line, &bufsize, stdin) == -1)
      if(!feof(stdin))
        perror("readline");

    // Finds the number of arguments
    args = parse(line, argv);

    

    // Executes the commands given by the list of arguments
    status = execute(args, argv);
  }
  exit(EXIT_SUCCESS);
}
