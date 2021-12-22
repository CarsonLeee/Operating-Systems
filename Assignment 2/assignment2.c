#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 4) //Checks if user passed X, Y, and Z. If not, program exits
    {
        printf("Invalid arguments\n");
        exit(0);
    }

    int port[2];
    pid_t pid, pid1_child, pid1_parent;
    char xValue[100], yValue[100], zValue[100];

    if(pipe(port) < 0){ //Checks if pipe failed
        printf("Error, pipe was not successful\n");
        exit(0);
    }

    pid = fork(); //Parent proces creates a child process

    if(pid < 0){ //Checks if pid was successful
        printf("Error, fork was unsuccessful\n");
        exit(0);
    }

    if(pid > 0){ //Checks if pid is in the parent
        pid1_child = pid;
        pid1_parent = getpid();
        printf("parent (PID %d) created a child (PID %d)\n", pid1_parent, pid1_child); //Prints line 1
        strcpy(xValue, argv[1]);  //Parent process reads first command argument into child process
        printf("parent (PID %d) receives X=\"%s\" from the user\n", pid1_parent, xValue); //Prints line 2
        write(port[1], xValue, 100); //The parent process will write X to the pipe
        printf("parent (PID %d) writes X=\"%s\" to the pipe\n", pid1_parent, xValue); //Prints line 3
        wait(NULL); //Waits for child process to finish
        read(port[0], zValue, 100); //Reads Z' from the pipe
        printf("parent (PID %d) reads concatenated result from the pipe (Z'=\"%s\")\n", pid1_parent, zValue);
    }

    if(pid == 0){ //Checks if pid is in the child
        read(port[0], xValue, 100); //Reads x from pipe written by parent, also helps to ensure program always begins in the parent
        strcpy(yValue, argv[2]); //Child process reads second arguments into variable Y
        strcpy(zValue, argv[3]); //Child process reads third arguments into variable X
        pid1_child = getpid();
        pid1_parent = getppid();
        printf("child (PID %d) receives Y=\"%s\" and Z=\"%s\" from the user\n", pid1_child, yValue, zValue);
        strcat(yValue, " "); //Adds space between the concatenated strings
        strcat(yValue, zValue); //Concatenates Y and Z to generate Y'
        printf("child (PID %d) concatenates Y and Z to generate Y'=\"%s\"\n", pid1_child, yValue);
        printf("child (PID %d) reads X from pipe=\"%s\"\n", pid1_child, xValue);
        strcat(xValue, " "); //Adds space between the concatenated strings
        strcat(xValue, yValue); //Concatenates X and Y' to generate Z'
        printf("child (PID %d) concatenates X and Y' to generate Z'=\"%s\"\n", pid1_child, xValue);
        printf("child (PID %d) writes Z' into the pipe\n", pid1_child);
        write(port[1], xValue, 100); //Writes Z' back into pipe for parent to read
    }
    return 0;
}
