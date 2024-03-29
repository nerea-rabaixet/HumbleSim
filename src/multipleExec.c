
// Multiple executions for HumbleSim
// Copyright (c) 2023, Universitat Pompeu Fabra.
//
// This file is provided "as is" without warranty of any kind. Please see the file LICENCE for more details.
//
// Authors: Nerea Rabaixet and Costas Michaelides
// Date: 2023-6-26

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

int main(void) {
    int i, fd, lock, size, status;
    char cmd[50], *result;
    FILE *fp;

    // Open CSV file for writing and write header
    fp = fopen("results_mult.csv", "w");
    if (fp == NULL) {
        printf("Error creating file!\n");
        exit(1);
    }
    fprintf(fp, "i,result\n");

    // Execute shell command for values of i from 1 to 10
    for (i = 10; i <= 100; i+=10) {
      for(int j=1;j<=100;j++){
        // Acquire lock on CSV file
        fd = fileno(fp);
        lock = flock(fd, LOCK_EX);
        if (lock == -1) {
            printf("Error acquiring lock!\n");
            exit(1);
        }

        // Execute shell command and read result

        sprintf(cmd, "./HumbleSim 3600 1 1 %d %d 1 1 > temp.txt", i, j);
        printf("./HumbleSim 3600 1 1 %d %d 1 1\n", i,j);
        status = system(cmd);
        if (status != 0) {
            printf("Error executing command!\n");
            exit(1);
        }

        // Read contents of temporary file into memory
        FILE *temp = fopen("temp.txt", "r");
        fseek(temp, 0L, SEEK_END);
        size = ftell(temp);
        fseek(temp, 0L, SEEK_SET);
        result = malloc(size + 1);
        fread(result, size, 1, temp);
        result[size] = '\0';
        fclose(temp);

        // Enclose result in double quotes and write to CSV file
        fprintf(fp, "%d,\"%s\"", i, result);

        // Release lock on CSV file
        lock = flock(fd, LOCK_UN);
        if (lock == -1) {
            printf("Error releasing lock!\n");
            exit(1);
        }

        // Print result to console
        printf("Result for i=%d and j=%d: %s", i, j,result);

        // Free result buffer
        free(result);

      }

    }

    // Close CSV file
    fclose(fp);

    return 0;
}
