//==============================================================================
//C Code for pthread_create() test
//==============================================================================

// ~0.570s

// Allows the use of pthreads
#include <pthread.h>
// Allows general IO including printf
#include <stdio.h>
// General c library, includes exit()
#include <stdlib.h>

#define NTHREADS 50000

// Declares the variable i and then assigns the value to 0 and then terminates the thread running this function
void *do_nothing(void *null) {
  int i;
  i=0;
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  // Variable declaration
  int rc, i, j, detachstate;
  // create an thread id object used to create a thread,
  // used to indicate a thread
  // uses pthread.h
  pthread_t tid;
  // create an thread attribute object 
  // used to determine what attibutes a thread may have, which includes detach state, stack size, etc.
  // uses pthread.h
  pthread_attr_t attr;

  // Initializes the thread attribute object and then sets the detach state to joinable.
  // This means that any thread created with the attribute object will be created in a joinable state.
  // Threads with a joinable state means that it will wait at the join call to free up resources
  // uses pthread.h
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  for (j=0; j<NTHREADS; j++) {
    // create a thread with the a thread the corresponding thread id object and the thread attribute object
    // this creates a thread in a joinable state,
    // this means the thread will stop at the join function to free up the resources it was using 
    // then the thread runs the do_nothing function
    // no arguments are passed to to the do_nothing function
    rc = pthread_create(&tid, &attr, do_nothing, NULL);
    // if an error occurs when trying to create a thread, its error code will be outputted
    // pthread_create function returns the error code if an error occurs while attempting to create a thread
    // then terminates the program, with -1 indicating that an error has occured
    if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
    // Main thread waits until thread created terminates
    // Frees up resources used by the thread
    rc = pthread_join(tid, NULL);
    // if an error occurs while trying to join the thread, the error code will be outputed
    // pthread_join function returns the error code if an error occurs while attempting to join the threads
    // then terminates the program, with -1 indicating that an error has occured
    if (rc) {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
      }
    }

    // frees up the resources used by the thread attribute object and terminates the current thread
    pthread_attr_destroy(&attr);
    pthread_exit(NULL);

}
