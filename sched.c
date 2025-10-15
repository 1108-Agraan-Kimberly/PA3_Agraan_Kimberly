#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
//#include "print_progress.c"

typedef struct _thread_data_t {
int localTid;
const int *data;
int numVals;
pthread_mutex_t *lock;
long long int *totalSum;
} 
thread_data_t;

void* arraySum(void*);
void print_progress(int localTid, size_t value);

int main(int argc, char* argv[]){

    if(argc != 2){
        printf("Not enough arguments. Usage: %s <num_threads> \n", argv[0]);
        return -1;
    }

     int numThreads = atoi(argv[1]);
    if (numThreads <= 0) {
        printf("Number of threads must be positive.\n");
        return -1;
    }

    int* bigArray = malloc(2000000 * sizeof(int));
    if (!bigArray) {
        printf("Failed to allocate array.\n");
        return -1;
    }

    long long int totalSum = 0;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);


        thread_data_t* tdata = malloc(numThreads * sizeof(thread_data_t));
    if (!tdata) {
        printf("Failed to allocate thread data.\n");
        free(bigArray);
        return -1;
    }

        for (int i = 0; i < numThreads; i++) {
        tdata[i].localTid = i;
        tdata[i].data = bigArray;
        tdata[i].numVals = 2000000;
        tdata[i].lock = &lock;
        tdata[i].totalSum = &totalSum;
    }

    pthread_t* threads = malloc(numThreads * sizeof(pthread_t));
    if(!threads){
        printf("Failed to allocate threads.\n");
        free(bigArray);
        free(tdata);
        return -1;
    }

    for(int i = 0; i < numThreads; i++){
        pthread_create(&threads[i], NULL, arraySum, &tdata[i]);
    }
     
    for(int i = 0; i < numThreads; i++){
        pthread_join(threads[i], NULL);
    }

    free(bigArray);
    free(tdata);
    free(threads);
    pthread_mutex_destroy(&lock);

    return 0;
}

void* arraySum(void* arg){

    thread_data_t* tdata = (thread_data_t*) arg;

    while(1){
        long long sum = 0;
        long latencyMax = 0;

        for(int i =0; i < tdata->numVals; i++){
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            sum += tdata->data[i];
            clock_gettime(CLOCK_MONOTONIC, &end);

            long latency = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);
            
            if(latency > latencyMax){
                latencyMax = latency;
            }
        }

        pthread_mutex_lock(tdata->lock);
        *(tdata->totalSum) += sum;
        pthread_mutex_unlock(tdata->lock);
        print_progress(tdata->localTid, latencyMax);

    }

    return NULL;
    
}