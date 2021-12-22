#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
  
int main() {  
    //Start reading each of the lines within the file. Every line represents a different set of processes to complete
    FILE *fp;
    char str[100];
    fp = fopen("rr_input.txt", "r");
    //We loop as long as there are lines to read
    while(fscanf(fp, "%s", str) != EOF) {
        //For the next line, we now go through every string (separated by whitespace) to read all times and the quantum time at the end.
        int at[100], bt[100], wt[100], tt[100], rem_bt[100], qt;
        bool done = false;
        int num = 0;
        while (!done) {
            //For the next token, we check if we have a new process or the final quantum time
            if(strncmp(str, "p", 1) == 0 || strncmp(str, "P", 1) == 0) {
                //This means we do have a process, so we read its arrival time and burst time
                fscanf(fp, "%d", &at[num]);
                fscanf(fp, "%d", &bt[num]);
                rem_bt[num] = bt[num];
                //We increase the num of processes so far and read next token
                num++;
                fscanf(fp, "%s", str);
            } else {
                //This means we're reading the final quantum time
                qt = atoi(str);
                done = true;
            }
        }
        //We start declaring variables to store the total turnaround times, waiting times and remaining processes to be completed. Included boolean to check if process was completed on a given round
        int sum_tt = 0, sum_wt = 0, rem_proc = num;
        bool completed = false;
        //Now, we loop in order to perform the round-robin algorithm for all processes
        int sum = 0, i = 0;
        while(rem_proc != 0) {  
            //Check if the next process can be completed in the current round
            if(rem_bt[i] <= qt && rem_bt[i] > 0) {
                //Increase sum, set rem burst time to zero and set completed as true
                sum += rem_bt[i];  
                rem_bt[i] = 0;  
                completed = true;
            } else if(rem_bt[i] > 0) {
                //Not possible yet to complete, so update time and increase sum
                rem_bt[i] -= qt;  
                sum += qt;
            }
            //Now, check if the process was completed in this round
            if(rem_bt[i] == 0 && completed) {
                //If so, we first store the waiting time and turnaround time for current process
                wt[i] = sum - at[i] - bt[i];
                tt[i] = sum - at[i];
                //Now, we decrease the number of processes still to complete and we increase the quantities needed for waiting and turnaround total times
                rem_proc--;
                sum_wt += wt[i];
                sum_tt += tt[i];
                completed = false;     
            } 
            //Finally, for round-robin purposes, update i index to go to next process to handle
            if(i == num-1) {  
                i = 0; //back to first if we're on last already
            } else if(at[i+1] <= sum) {  
                i++;   //next process in list because arrival time was reached
            } else {  
                i = 0; //back to first if next hasn't started yet
            }
        }  
        //Print the results for each once all processes are completed 
        for(i = 0; i < num; i++) {
            printf("Process: P%d Arrival Time: %d Burst time: %d Waiting Time: %d Turnaround Time: %d\n", i+1, 
                at[i], bt[i], wt[i], tt[i]);
        }
        //Now, we print the final stats
        printf("\nTotal Turnaround Time: %d\n", sum_tt);
        printf("Average waiting time = %.2f\n", (sum_wt*1.0)/num);
        printf("Average turnaround time = %.2f\n\n", (sum_tt*1.0)/num);
    }
    //Once all lines were parsed, we close the file pointer
    fclose(fp);
    return 0;
}  
