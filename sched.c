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
void print_progress(pid_t localTid, size_t value) {
        pid_t tid = syscall(__NR_gettid);

        TERM_GOTOXY(0,localTid+1);

	char prefix[256];
        size_t bound = 100;
        sprintf(prefix, "%d: %ld (ns) \t[", tid, value);
	const char suffix[] = "]";
	const size_t prefix_length = strlen(prefix);
	const size_t suffix_length = sizeof(suffix) - 1;
	char *buffer = calloc(bound + prefix_length + suffix_length + 1, 1);
	size_t i = 0;

	strcpy(buffer, prefix);
	for (; i < bound; ++i)
	{
	    buffer[prefix_length + i] = i < value/10000 ? '#' : ' ';
	}
	strcpy(&buffer[prefix_length + i], suffix);
        
        if (!(localTid % 7)) 
            printf(ANSI_COLOR_WHITE "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 6)) 
            printf(ANSI_COLOR_BLUE "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 5)) 
            printf(ANSI_COLOR_RED "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 4)) 
            printf(ANSI_COLOR_GREEN "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 3)) 
            printf(ANSI_COLOR_CYAN "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 2)) 
            printf(ANSI_COLOR_YELLOW "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 1)) 
            printf(ANSI_COLOR_MAGENTA "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else
            printf("\b%c[2K\r%s\n", 27, buffer);

	fflush(stdout);
	free(buffer);
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