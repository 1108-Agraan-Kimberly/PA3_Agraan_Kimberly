#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

int executeCommand(char* const*, const char*, const char*);
void changeDirectories(const char*);
int parseInput(char*, char [], int);

int main(){


    return 0;
}

int executeCommand(char* const* enterCommand, const char* infile, const char* outfile){

    

}

void changeDirectories(const char* path){

    if(chdir(path) != 0){
        printf("error code: %d\n", errno);
        printf("error: %s\n", strerror(errno));
    }

}

int parseInput(char* input, char splitWords[], int maxWords){
    strtok(input, " \n");
    while(input != NULL && maxWords > 0){
        strcpy(splitWords, input);
        splitwords += strlen(input) + 1;
        maxWords--;
        input = strtok(NULL, " \n")
    }
    return maxWords;
}