#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "print_progress.c"

typedef struct _thread_data_t {
int localTid;
const int *data;
int numVals;
pthread_mutex_t *lock;
long long int *totalSum;
} 
thread_data_t;

void* arraySum(void*);

int main(int argc, char* argv[]){



}

void* arraySum(void* arg){

    thread_data_t* tdata = (thread_data_t*) arg;

    while(1){
        long long sum = 0;
        long latencyMax = 0;

        for(int i =0; i < tdata->numVals; i++){
            struct timespec start end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            threadSum += tdata->data[i];
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