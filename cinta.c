// cinta.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // include pthread library for multithreading
#include <sys/ipc.h>
#include <sys/shm.h>

#define ROW1 4
#define COL2 5

// function to calculate factorial of a number
void* calculate_factorial(void* arg) {
    int num = *(int*) arg; // cast argument back to integer
    long double* result = malloc(sizeof(long double)); // allocate memory for result

    // calculate factorial
    long double factorial = 1;
    for (int i = 1; i <= num; i++) {
        factorial *= i;
    }

    *result = factorial; // store result in allocated memory
    pthread_exit(result); // exit thread and return result
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
    printf("Result Matrix cinta:[\n");
    for (int i = 0; i < ROW1; i++) {
        printf("\t[");
        for (int j = 0; j < COL2; j++) {
            printf("%d,", res[i * COL2 + j]);
        }
        printf("], \n");
    }
    printf("]\n");

    // display factorial matrix
    printf("Factorial Matrix cinta:\n");
    for (int i = 0; i < ROW1; i++) {
        for (int j = 0; j < COL2; j++) {
            // create new thread for each number in matrix and pass number to calculate_factorial function
            pthread_t tid;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            int* num = &res[i * COL2 + j];
            pthread_create(&tid, &attr, calculate_factorial, (void*) num);

            // wait for thread to finish and print out result
            void* result;
            pthread_join(tid, &result);
            long double factorial = *(long double*) result;
            printf("%.0Lf ", factorial);

            free(result); // free allocated memory
        }
        printf("\n");
    }

    // detach and remove shared memory
    shmdt(res);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
