#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep

//Written by James Levi, Florida Atlantic University.

//struct to store the information which is needed in the doWork function. A pointer to this struct will be casted onto doWork's paramater void*arg.
struct important_info {
    pthread_mutex_t mutex;
    pthread_cond_t cond_var;
    int count;
};

// global variables here:
const int M=3; //Declare M as a global integer variable and hard-code its value to 3.


void* doWork(void* arg);

void enter(struct important_info* info); //uses mutex lock to increment the count of how many threads are in the Msection. Uses a conditional variable to ensure that access to the Msection is only granted when there are less than 3 threads in the Msection. If not, it waits until the conditional variable allows the thread to proceed.

void doCriticalWork(struct important_info* info); //prints the thread Id and count (how many threads inside the Msection)

void leave(struct important_info* info); //uses mutex lock to decrement the count, and to signal to the wait.

int main() {
    long n = 10; //# of threads
    pthread_attr_t attr;
    pthread_t *vtid = (pthread_t *) calloc(n, sizeof(pthread_t));
    if (vtid == NULL) {
        printf("ERROR: calloc failed\n\n");
        exit(6);
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    //declaring the struct. Done locally as per the instructions so that the enter and leave functions do not rely on global variables.

    struct important_info main_info;
    main_info.count = 0; // Initialize count 
    pthread_mutex_init(&main_info.mutex, NULL); //initialize mutex lock
    pthread_cond_init(&main_info.cond_var, NULL); //initialize conditional variable.

   //creating the 10 threads and making them run doWork()
    for (long i = 0; i < n; i++) {
        int r = pthread_create(&vtid[i], &attr, doWork, (void*)&main_info);
        if (r < 0) {
            printf("ERROR: pthread_create() returned %d\n", r);
            exit(2);
        }
    }

   // now wait for the threads to complete:
    for (long i = 0; i < n; i++) {
        pthread_join(vtid[i], NULL);
    }

    //resource handling.
    printf("All threads finished\n");
    pthread_attr_destroy(&attr);
    free(vtid);
    pthread_mutex_destroy(&main_info.mutex);
    pthread_cond_destroy(&main_info.cond_var);

    return 0;
}

void* doWork(void* arg) {

        //since the thread_create does not allow multiple paramters (mutex lock, conditional variable, and count), the structure was created to hold all three, and is then casted onto the arg.
    struct important_info* thread_info = (struct important_info*) arg;
    for (int i = 0; i < 6; i++) {
        //calls the helper functions
        enter(thread_info);
        doCriticalWork(thread_info);
        leave(thread_info);
    }
    return NULL;
}


void enter(struct important_info* info) {

    //The mutex lock is to ensure count is not simultaneously changed. The count (alongside the conditional varible) assures that at most 3 threads are in the m-section at a time.

    pthread_mutex_lock(&info->mutex); //attempts to lock the mutex. If it is successful (so if it was unlocked), it continues, if not, it waits.

    while (info->count >= M) {
        pthread_cond_wait(&info->cond_var, &info->mutex);//even if it got the mutex, the threads waits if there are already 3 threads in the Msection. When waiting, it sleeps (does not use CPU) and releases the mutex. It waits for a signal to be awoken, and when it wakes up it reaquires the mutex.
    }

    info->count++; //increments the count, mutually exclusively
    pthread_mutex_unlock(&info->mutex); //unlocks the mutex, allowing another thread to attempt to increment count and enter the Msection.

}

void leave(struct important_info* info) {

    pthread_mutex_lock(&info->mutex); //lock mutex to ensure that count is properly decremented
    info->count--;
    pthread_cond_signal(&info->cond_var);//signals to the cond_wait to wake up, since count decremented there must be an available spot in the Msection.
    pthread_mutex_unlock(&info->mutex); //unlocks the mutex
}

void doCriticalWork(struct important_info* info) {
    
    pthread_t thread_id = pthread_self();
    //output
    printf("Thread ID: %lu \n Number of Threads in M-Section: %d \n \n", (unsigned long) thread_id, info->count);

    usleep(1000);//to avoid issue where a thread continuously gains access to the critical section, set a delay to give other threads access
}