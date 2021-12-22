#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>


int port[2];
char xValue[20];
char yValue[20];
char S[20];


//Function is executed by thread 100, reads X and Y from pipe
void *sum(void *thread_id){
    
    int *id = (int*)thread_id; //For thread ID
    read(port[0], &xValue, 4); //Reads x value from pipe
    read(port[0], &yValue, 4); //Reads y value from pipe
    
    printf("thread (TID %d) reads X = %s and Y = %s from the pipe\n", *id, xValue, yValue);
    int xInt = (int)atoi(xValue); //Converts string into integer
    int yInt = (int)atoi(yValue); //Converts string into integer
    int S = xInt + yInt; //Adds the integers together into one string
    printf("thread (TID %d) writes X+Y = %d to the pipe\n", *id, S);
    write(port[1], &S, 4); //Writes value of S back to the pipe
    pthread_exit(NULL);
    return 0;
}

void *odd_even(void *thread_id){
    int *id = (int*)thread_id;
    read(port[0], &S, 4);
    
    printf("thread (TID %d) reads X+Y = %i from the pipe\n", *id, *S);
    
    //If statement to check if it is even or odd
    if(*S%2 != 0){
        printf("thread (TID %d) identifies X+Y = %i as an odd number\n", *id, *S);
    }else{
        printf("thread (TID %d) identifies X+Y = %i as an even number\n", *id, *S);
    }
    
    printf("thread (TID %d) writes X+Y = %i to the pipe\n", *id, *S);
    write(port[1], &S, 4);
    pthread_exit(NULL);
    return 0;
}

void *digit_count(void *thread_id){
    int *id = (int*)thread_id;
    read(port[0], &S, 4);
    printf("thread (TID %d) reads X+Y = %d from the pipe\n", *id, *S);
    
    int temp = *S;
    int digits = 0;
    int n = 0;
    while(temp){
        temp = temp/10;
        n++;
    }
    digits = n;

    printf("thread (TID %d) identifies X+Y = %d as a %d digit number\n", *id, *S, digits);
    pthread_exit(NULL);
    return 0;
}

int main(int argc, char **argv){
    
    if(argc != 3){ //Checks if user passed X and Y
        printf("Invalid arguments\n");
        exit(0);
    }
    
    int result = pipe(port);
    if(result < 0){ //Checks if pipe is successful
        perror("pipe error\n");
        exit(0);
    }
    
    strcpy(xValue, argv[1]); //Gets x value from parent
    strcpy(yValue, argv[2]); //Gets y value from parent
    printf("parent (PID %d) receives X = %s and Y = %s from the user\n", getpid(), xValue, yValue);
    
    printf("parent (PID %d writes X = %s and Y = %s to the pipe\n", getpid(), xValue, yValue);
    
    write(port[1], xValue, 4); //Writes x to pipe
    write(port[1], yValue, 4); //Writes y to pipe
    
    pthread_t threadZero; //Thread 100
    int thread_id_0 = 100;
    pthread_create(&threadZero, NULL, sum, &thread_id_0);
    pthread_join(threadZero, NULL);
    
    pthread_t threadOne; //Thread 101
    int thread_id_1 = 101;
    pthread_create(&threadOne, NULL, odd_even, &thread_id_1);
    pthread_join(threadOne, NULL);
    
    pthread_t threadTwo; //Thread 102
    int thread_id_2 = 102;
    pthread_create(&threadTwo, NULL, digit_count, &thread_id_2);
    pthread_join(threadTwo, NULL);
     
    return 0;
}
