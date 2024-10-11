#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//writen by James Levi, Florida Atlantic University. 
//This program uses barriers to show how concurrent threads must wait for eachother to reach a barrier before they can proceed. The barriers use mutex lock and conditional variable, along side an int count and a bool trapped, to control this. However, I noticed that, in this case, it is simpler to not use reusable barriers. Since the logic relies on the count and trapped variable to release the threads from the barrier, no method was found to be able to reset the count and trapped variables to trap the threads for the next barrier. THis is because when a thread gets released from a barrier, it signals to the other threads that they are not trapped (sets trapped to false) and imediately goes to the next barrier. However, if we then set trapped to true for the next barrier, the threads that were not quick enough to leave the first barrier will already be trapped because of this. So, to summarize, the issue was was that a barrier could not be reset, so multiple barriers were created.

// Barrier class definition
class Barrier {
public:
    Barrier(int n);
    ~Barrier();
    void wait();

private:
    pthread_mutex_t mutex;
    pthread_cond_t cond_var;
    int count; //counts the threads that have made it to the barrier
    int number_of_threads; //total number of threads, the count must reach this number to proceed
    bool trapped; //once count==number_of_threads, trapped= false for this baricade, they are et free.
};

Barrier::Barrier(int n) {
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "Error initializing mutex\n");
        exit(1);
    }
    if (pthread_cond_init(&cond_var, NULL) != 0) {
        fprintf(stderr, "Error initializing condition variable\n");
        exit(1);
    }
    count = 0;
    number_of_threads = n;
    trapped=true;
}

Barrier::~Barrier() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_var);
}

void Barrier::wait() {
    pthread_mutex_lock(&mutex); //lock mutex.

    if (++count == number_of_threads) {//if this thread is the last thread
        trapped=false; //set trapped to false, everyone is free
        pthread_cond_broadcast(&cond_var); // Wake up all waiting threads because trapped was changed
    } else {
        while (trapped) {//else, wait until trapped is changed
            pthread_cond_wait(&cond_var, &mutex);
        }
    }

    pthread_mutex_unlock(&mutex);//unlock
}




// Global variables
const int num_threads = 10;
//multiple barriers are created since they are not reusable.
Barrier barrier1(num_threads);
Barrier barrier2(num_threads);
Barrier barrier3(num_threads);

// Thread function
void* threadFunction(void* arg) {
    pthread_t thread_id = pthread_self();

        printf("Thread %lu has reached the barrier 1!\n", (unsigned long)thread_id);
        barrier1.wait();
        printf("Thread %lu has reached the barrier 2!\n", (unsigned long)thread_id);
        barrier2.wait();
        printf("Thread %lu has reached the barrier 3!\n", (unsigned long)thread_id);
        barrier3.wait();
        printf("Thread %lu has reached the end !\n", (unsigned long)thread_id);
    return NULL;
}

int main() {
    pthread_attr_t attr;
    pthread_t vtid[num_threads];

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < num_threads; i++) {
        int r = pthread_create(&vtid[i], &attr, threadFunction, NULL);
        if (r != 0) {
            fprintf(stderr, "ERROR: pthread_create() returned %d\n", r);
            exit(1);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(vtid[i], NULL);
    }

    pthread_attr_destroy(&attr);

    return 0;
}