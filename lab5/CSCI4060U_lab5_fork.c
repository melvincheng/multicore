//==============================================================================
//C Code for fork() creation test
//==============================================================================

// ~4.000s

// Allows general IO including printf
#include <stdio.h>
// General c library, includes exit()
#include <stdlib.h>
// Allows the use of forks and pipes
#include <unistd.h>
// Allows the use of the process type
#include <sys/types.h>
// Allows the use of waitpid
#include <sys/wait.h>

#define NFORKS 50000

// Declares the variable i and then assigns the value to 0
void do_nothing() {
  int i;
  i= 0;
}

int main(int argc, char *argv[]) {
  // Variable declaration
  int pid, j, status;
  // create a process id object used to indicate a process
  // uses the sys/types.h
  pid_t processID;

  for (j=0; j<NFORKS; j++) {
    // Fork attempts to create a copy of the program
    // if fork fails to create a copy, print an error statement with the error number
    // if a child process is successfully created, the return value of fork would be the process id of the child
    // then exit the program
    // the child which would be a copy of the program at the time of the fork call, would have the return value of 0 instead
    // any value smaller than 0 indicates that a error occured when the attempting to create a child process
    // the fork function uses sys/types.h
    if ((processID = fork()) < 0 ) {
      printf ("fork failed with error code= %d\n", processID);
      exit(0);
    }
    // if process is a child process, run the do_nothing function and then exit
    // if the processID is 0, that means the process is a child a process,
    // as fork will return a 0 if the current process is a child process
    else if (processID ==0) {
      do_nothing();
      exit(0);
    }
    // if forking is successful,
    // the current process waits until any child processes with the corresponding processID, to terminate
    // because the final argument in waitpid is 0, that indicates that the current process will wait for any child with the corresponding ID
    // waitpid uses sys/wait.h
    else {
      waitpid(processID, &status, 0);
    }
  }
}
