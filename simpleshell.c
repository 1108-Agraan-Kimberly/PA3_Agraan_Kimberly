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

int executeCommand(char* const*, const char*, const char*);
void changeDirectories(const char*);
int parseInput(char*, char [], int);

int main(){
 char input[256];
    char splitWords[20][128]; // Up to 20 words, each up to 127 chars
    char* parsedWords[20];    // Array of pointers to each word
    char cwd[256];
    const char* netid = "yourNetID"; // <-- Replace with your actual netid

    while (1) {
        // Display prompt
        getcwd(cwd, sizeof(cwd));
        printf("%s:%s$ ", netid, cwd);

        // Get user input
        if (!fgets(input, sizeof(input), stdin)) break;

        // Parse input into splitWords buffer
        int wordCount = 0;
        char* token = strtok(input, " \n");
        while (token && wordCount < 20) {
            strcpy(splitWords[wordCount], token);
            parsedWords[wordCount] = splitWords[wordCount];
            wordCount++;
            token = strtok(NULL, " \n");
        }
        parsedWords[wordCount] = NULL; // Null-terminate for execvp

        if (wordCount == 0) continue; // Empty input

        // Handle 'exit'
        if (strcmp(parsedWords[0], "exit") == 0) {
            break;
        }

        // Handle 'cd'
        if (strcmp(parsedWords[0], "cd") == 0) {
            if (wordCount > 1) {
                changeDirectories(parsedWords[1]);
            } else {
                printf("No directory specified.\n");
            }
            continue;
        }

        // Check for redirection
        int redirectIdx = -1;
        char infile[128] = "";
        char outfile[128] = "";
        for (int i = 0; i < wordCount; i++) {
            if (strcmp(parsedWords[i], "<") == 0) {
                redirectIdx = i;
                if (i + 1 < wordCount) strcpy(infile, parsedWords[i + 1]);
                break;
            }
            if (strcmp(parsedWords[i], ">") == 0) {
                redirectIdx = i;
                if (i + 1 < wordCount) strcpy(outfile, parsedWords[i + 1]);
                break;
            }
        }

        // Prepare command array for execvp
        int cmdCount = (redirectIdx == -1) ? wordCount : redirectIdx;
        char** execArgs = malloc((cmdCount + 1) * sizeof(char*));
        for (int i = 0; i < cmdCount; i++) {
            execArgs[i] = parsedWords[i];
        }
        execArgs[cmdCount] = NULL;

        // Execute command
        executeCommand(execArgs, infile[0] ? infile : NULL, outfile[0] ? outfile : NULL);

        free(execArgs);
    }
    
    return 0;
}

int executeCommand(char* const* enterCommand, const char* infile, const char* outfile){
    pid_t pid = fork();
    if(pid < 0){
        printf("error: %s\n", strerror(errno));
        return 1;
    }
    if (pid == 0){
        if(infile && infile[0] != '\0'){
            int fd = open(infile, O_RDONLY);
            if(fd < 0){
                printf("error: %s\n", strerror(errno));
                _exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if(outfile && outfile[0] != '\0'){
            int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if(fd < 0){
                printf("error: %s\n", strerror(errno));
                _exit(1);
            }
            dup2(fd, stdout_fileno);
            close(fd);
        }
        execvp(enterCommand[0], enterCommand);
        printf("error: %s\n", strerror(errno));
        _exit(1);
    }   
    else{
        int status;
        wait(&status);
        if(WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            printf("Child finished with error status: %d\n", WEXITSTATUS(status));
            return 1;
        }
    }
    return 0;
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
        splitWords += strlen(input) + 1;
        maxWords--;
        input = strtok(NULL, " \n");
    }
    return maxWords;
}