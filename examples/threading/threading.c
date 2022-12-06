#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param) //thread start routine
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    struct thread_data* mythread = (struct thread_data *) thread_param;
sleep((mythread-> wait_to_obtain_ms)/1000);
pthread_mutex_lock(&mythread->mutex);
sleep((mythread->wait_to_release_ms)/1000);
pthread_mutex_unlock(&mythread->mutex);
pthread_exit(mythread->thread_ret_value);
return thread_param;
}


/**
* Start a thread which sleeps @param wait_to_obtain_ms number of milliseconds, then obtains the
* mutex in @param mutex, then holds for @param wait_to_release_ms milliseconds, then releases.
* The start_thread_obtaining_mutex function should only start the thread and should not block
* for the thread to complete.
* The start_thread_obtaining_mutex function should use dynamic memory allocation for thread_data
* structure passed into the thread.  The number of threads active should be limited only by the
* amount of available memory.
* The thread started should return a pointer to the thread_data structure when it exits, which can be used
* to free memory as well as to check thread_complete_success for successful exit.
* If a thread was started succesfully @param thread should be filled with the pthread_create thread ID
* coresponding to the thread which was started.
* @return true if the thread could be started, false if a failure occurred.
*/

bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
int* thread_pointer;
struct thread_data thread1;
//struct thread_data thread2;
thread1.mutex=*mutex;
pthread_mutex_init(&thread1.mutex,NULL);
thread_pointer=(int*) malloc(100*sizeof(thread1));
//thread_pointer=(int*) malloc(sizeof(thread2));
thread1.wait_to_obtain_ms=wait_to_obtain_ms;
//thread2.wait_to_obtain_ms=wait_to_release_ms;
thread1.ret=pthread_create(thread,NULL, threadfunc,&thread1.thread);
//thread2.thread_ret_value=pthread_create(&thread2.thread,NULL, threadfunc,NULL);
if (thread1.ret==true) 
{
thread1.thread_complete_success=true;
thread1.thread=pthread_self();
}
else thread1.thread_complete_success=false;
bool thread_complete_success=thread1.thread_complete_success;
//pthread_exit(thread1.thread_ret_value);
free(thread_pointer);
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
 return thread_complete_success;
}

