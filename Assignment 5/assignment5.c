#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

//Semaphore object to create the mutex behavior
sem_t mutex;

//Shared resources for accounts and transactions (100 as max num of accounts and clients)
int accounts[100];
char all_transactions[100][100][50];
int num_transactions[100];

//Function that performs the given transaction within the bank accounts
void* performTransactions(void *arg) {
    int num_client = (int *)arg;

    //Loop through client transactions and perform the needed actions
    int i = 0;
    while(i < num_transactions[num_client]*3) {
        //To process next transaction, we first wait for the semaphore
        sem_wait(&mutex);
        //Once we're given access to shared resources (accounts), we get transaction type, account and value
        char trans_type = all_transactions[num_client][i][0];
        int acc = atoi(all_transactions[num_client][i+1]+1)-1;
        int amt = atoi(all_transactions[num_client][i+2]);
        //Checks type of transaction
        if(trans_type == 'w' || trans_type == 'W') {
            //Withdrawal should be made. Check that the amount needed is available. If so, we do the withdrawal
            if(accounts[acc] - amt >= 0) {
                accounts[acc] -= amt;
            }
        } else {
            //A deposit should be made. We check that the deposit amount is valid (meaning the final balance becomes positive). If so, we do the deposit
            if(accounts[acc] + amt >= 0) {
                accounts[acc] += amt;
            }
        }
        //Release the semaphore and increase i index for next transactions
        sem_post(&mutex);
        i += 3;
    }
    return NULL;
}

//Main function that starts the Bank application, using Mutual Exclution for sync processes
int main() {  
    //Start reading each of the lines within the file. 
    FILE *fp;
    char str[100];
    fp = fopen("assignment_6_input.txt", "r");

    //The first line represents the amount of bank accounts and their initial balances. We read next token and loop until no more accounts are given
    bool done = false;
    int num = 0;
    fscanf(fp, "%s", str);
    while(!done) {
        if(strncmp(str, "a", 1) == 0 || strncmp(str, "A", 1) == 0) {
            //This means we have a new bank account
            fscanf(fp, "%s", str);
            fscanf(fp, "%d", &accounts[num]);
            //We increase the num of accounts and read next token
            num++;
            fscanf(fp, "%s", str);
        } else {
            //This means we just started reading transactions
            done = true;
        }
    }
    
    //The rest of the lines represent the clients transactions to be made. Here, we just read those transactions (client by client)
    int clients = 0;
    done = false;
    while(!done) {
        //We start a new client set of transactions. We read them all and store them in the previous defined arrays
        bool client_done = false;
        int i = 0;
        while(!client_done && !done) {
            if(fscanf(fp, "%s", str) == EOF) {
                //This means we reached the end of the file
                done = true;
                client_done = true;
            } else if(strncmp(str, "c", 1) == 0 || strncmp(str, "C", 1) == 0) {
                //Next client found
                client_done = true;
            } else {
                //Copy transaction details as needed
                strcpy(all_transactions[clients][i], str);
                i++;
            }
        }
        //Once the client was processed, remove trailing new line from last value, store num of transactions and increase num of clients
        all_transactions[clients][i][strcspn(all_transactions[clients][i], "\r\n")] = 0;
        num_transactions[clients] = i/3; // i/3 because there are 3 values per transaction
        clients++;
    }
    
    //Now, we init the semaphore mutex object and we create the threads needed, one for each client
    sem_init(&mutex, 0, 1);
    pthread_t ts[clients];
    for(int i = 0; i < clients; i++) {
        pthread_create(&ts[i], NULL, performTransactions, (void *)i);
    }

    //We wait for all threads to complete
    for(int i = 0; i < clients; i++) {
        pthread_join(ts[i], NULL);
    }

    //Once all transactions were processed, we print the final balances and close the file pointer
    for(int i = 0; i < num; i++) {
        printf("a%d b %d\n", i+1, accounts[i]);
    }
    fclose(fp);
    return 0;
}  
