#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define ROW1 4
#define COL2 5

// function to calculate factorial of a number
long double calculate_factorial(int num) {
    long double factorial = 1;
    for (int i = 1; i <= num; i++) {
        factorial *= i;
    }
    return factorial;
}

int main() {
    key_t key = ftok("kalian.c", 'R'); // generate a key based on kalian.c file
    if (key == -1) {
        perror("ftok error");
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(key, ROW1 * COL2 * sizeof(int), 0666 | IPC_CREAT); // get shared memory ID
    if (shmid == -1) {
        perror("shmget error");
        exit(EXIT_FAILURE);
    }

    int* res = (int*) shmat(shmid, NULL, 0); // attach shared memory
    if (res == (int*) -1) {
        perror("shmat error");
        exit(EXIT_FAILURE);
    }

    // display result matrix from shared memory
    printf("Result Matrix sisop:[\n");
    for (int i = 0; i < ROW1; i++) {
        printf("\t[");
        for (int j = 0; j < COL2; j++) {
            printf("%d,", res[i * COL2 + j]);
        }
        printf("], \n");
    }
    printf("]\n");

    // display factorial matrix
    printf("Factorial Matrix sisop:\n");
    for (int i = 0; i < ROW1; i++) {
        for (int j = 0; j < COL2; j++) {
            int num = res[i * COL2 + j];
            long double factorial = calculate_factorial(num);
            printf("%.0Lf ", factorial);
        }
        printf("\n");
    }

    // detach and remove shared memory
    shmdt(res);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
