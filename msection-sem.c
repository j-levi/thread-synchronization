#include <pthread.h>//thread library
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>// semaphore library
#include <unistd.h> // for usleep

// global variables here:
const int M=3; //Declare M as a global integer variable and hard-code its value to 3.

//Thread function
void* doWork(void* arg);

//Helper functions
void enter(sem_t* semaphore);//makes the thread wait until semaphore is greater than 0,thus if a thread can decrement the semaphore, it enters the critical section. 

void leave(sem_t* semaphore);//when leaving, the thread increments the semaphore value

void doCriticalWork(sem_t* semaphore); //Print the current thread id and the number of threads currently in the m- section. Only 3 threads may do critical work at a time.

int main()
{
   long n = 10;    // number of threads

   pthread_attr_t attr;
   pthread_t *vtid = (pthread_t *) calloc(n, sizeof(pthread_t));
  
   if (vtid == NULL) //error handling for calloc
   {
       printf("ERROR: calloc failed\n\n");
       exit(1);
   }
    
    sem_t local_semaphore; //declare semaphore, local to the main, so that the enter and leave functions do not depend on a global variable.

    sem_init(&local_semaphore,0,M); //pointer to the semaphore, flag for level of sharing, initial value. M is set to 3, indicating how many available spots there are in the critical section at a given time.

   //CREATING THE ATTRIBUTE
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   //creating the 10 threads and making them run doWork()
   for (long i = 0; i < n; i++)
   {
       int r = pthread_create(&vtid[i], &attr, doWork, (void*)&local_semaphore);
       if (r < 0)
       { //error hanlding for thread_create
           printf("ERROR: pthread_create() returned %d\n", r);
           exit(2);
       }
   }

   // now wait for the threads to complete:
   for (long i=0; i<n; i++)
   {
       pthread_join(vtid[i], NULL);
   }

   //resource handling.
   printf("all threads finished");
   pthread_attr_destroy(&attr);
   free(vtid);
   sem_destroy(&local_semaphore);
   return(0);
}

void* doWork(void* arg)
{
    //since the thread_create does not allow for the pointer of the semaphore to be passed as a paramater, this Casts arg to a semaphore pointer of type sem_t.
   sem_t* semaphore = (sem_t*)arg;

    for(int i=0;i<6;i++) //repeats 6 times, for demonstration purposes
   {
       enter(semaphore); // limit access to m threads

       doCriticalWork(semaphore); // run by max. m threads

       leave(semaphore); // leave m-section
   }
   return NULL;
}

void enter(sem_t* semaphore){
       sem_wait(semaphore); //attempts to decrement the semaphore, but if its at 0 (no spaces available in the critical section), it must wait.
}

void doCriticalWork(sem_t* semaphore)
{
   int sem_value;
   sem_getvalue(semaphore, &sem_value);//store the current value of the semaphore in sem_value. 

   pthread_t thread_id = pthread_self(); //get the thread id

    //output
    printf("Thread ID: %lu \n Number of Threads in M-Section %d \n \n", (unsigned long) thread_id,M - sem_value);

    //
    usleep(1000); //to avoid issue where a thread continuously gains access to the critical section, set a delay to give other threads access
}

void leave(sem_t * semaphore){
   sem_post(semaphore); //increments the semaphore, giving up its spot.
}