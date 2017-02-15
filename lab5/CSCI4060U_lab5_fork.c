//==============================================================================
//C Code for fork() creation test
//==============================================================================

// Allows general IO including printf
#include <stdio.h>
// General c library, includes exit()
#include <stdlib.h>
// Allows the use of forks and pipes
#include <unistd.h>
// Allows the use of the process type
#include <sys/types.h>

#define NFORKS 50000

// initializes the variable i and then set the value to 0
void do_nothing() {
  int i;
  i= 0;
}

int main(int argc, char *argv[]) {
  // Variable initialization
  int pid, j, status;
  pid_t processID;

  for (j=0; j<NFORKS; j++) {
    // if forking failed, print error code and then exit process 
    if ((processID = fork()) < 0 ) {
      printf ("fork failed with error code= %d\n", processID);
      exit(0);
    }
    // if process is a child, run the do_nothing function and then exit
    else if (processID ==0) {
      do_nothing();
      exit(0);
    }
    // if forking is successful, the parent process waits until the child process terminates
    else {
      waitpid(processID, &status, 0);
    }
  }
}
