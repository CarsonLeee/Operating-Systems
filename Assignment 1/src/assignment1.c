//Name: clee887
//Date: September 23, 2021
//Prof: Prof.Haque
//Description: Assignment 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv){
    
    if (argc != 2) { //Checks if user inputted value
        printf("Invalid arguments\n");
        exit(0);
    }
    pid_t pid, pid_parent, pid1_child, pid2_child, pid21_child;
    pid = fork();

    if(pid > 0){ //Checks to see if we are in parent or child_1
        wait(NULL); //Goes into child_1
        pid = fork(); //Creates child_2 after child1 is complete
        if(pid > 0){ //Checks to see if it is in parent or child_2
            wait(NULL); //Goes into child_2
            printf("child_1 and child_2 are completed and parent process is terminating...");
        }
        
        if(pid == 0){ //Inside child_2
            pid_parent = getppid(); //Gets parent PID
            pid2_child = getpid(); //Gets child2 PID
            printf(("parent (PID %d) created child_2 (PID %d)\n"), pid_parent, pid2_child);
            pid = fork();
            if(pid > 0){ //Inside child_2_parent
                wait(NULL);

            }
            if(pid == 0){ //Inside child_2.1
                pid2_child = getppid();
                pid21_child = getpid();
                printf("child_2 (PID %d) created child_2.1 (PID %d)\n", pid2_child, pid21_child);
                printf("child_2.1 (PID %d) is calling an external program external_program.out and leaving child_2\n", pid21_child);
                char pid21[20];
                int currPID = getpid();
                sprintf(pid21, "%d", currPID); //Combines together
                char* combine;
                combine = strcat(pid21, " for child_2.1"); //Concatenates together
                execl(argv[1], "external_program.out", pid21, NULL); //Path for external_program.out
            }
        }
    }
    
    if(pid == 0){ //We are now in child_1
        pid1_child = getpid();
        pid_parent = getppid();
        printf("parent (PID %d) created child_1 (PID %d)\n", pid_parent, pid1_child);
        printf("parent (PID %d) is waiting for child_1 (PID %d) to complete before creating child_2\n", pid_parent, pid1_child);
        printf("child_1 (PID %d) is calling an external program external_program.out and leaving parent\n", pid1_child);
        char pid1[20];
        int currPID = getpid();
        sprintf(pid1, "%d", currPID); //Combines together
        char* combine;
        combine = strcat(pid1, " for child_1"); //Concatenates together
        execl(argv[1], "external_program.out", pid1, NULL); //Path for external_program.out
    }
    return 0;
}
