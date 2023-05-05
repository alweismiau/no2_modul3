#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define ROW1 4
#define COL1 2
#define ROW2 2
#define COL2 5

int main() {
    srand(time(NULL));
    int mat1[ROW1][COL1], mat2[ROW2][COL2], res[ROW1][COL2];

    // Generate random numbers for matrix 1
    printf("Matrix 1:\n");
    for(int i=0; i<ROW1; i++) {
        for(int j=0; j<COL1; j++) {
            mat1[i][j] = rand() % 5 + 1;
            printf("%d ", mat1[i][j]);
        }
        printf("\n");
    }

    // Generate random numbers for matrix 2
    printf("\nMatrix 2:\n");
    for(int i=0; i<ROW2; i++) {
        for(int j=0; j<COL2; j++) {
            mat2[i][j] = rand() % 4 + 1;
            printf("%d ", mat2[i][j]);
        }
        printf("\n");
    }

    // Multiply matrices
    for(int i=0; i<ROW1; i++) {
        for(int j=0; j<COL2; j++) {
            res[i][j] = 0;
            for(int k=0; k<COL1; k++) {
                res[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }

    // Display result matrix
    printf("\nResult Matrix:\n");
    for(int i=0; i<ROW1; i++) {
        for(int j=0; j<COL2; j++) {
            printf("%d ", res[i][j]);
        }
        printf("\n");
    }

    // Shared memory key
    key_t key = ftok("kalian.c", 'R');
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }

    // Create shared memory segment
    int shmid = shmget(key, sizeof(res), 0644 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    // Attach to shared memory segment
    int *shm = (int*) shmat(shmid, NULL, 0);
    if (shm == (int*)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Copy matrix multiplication result to shared memory segment
    int i, j, k = 0;
    for(i=0; i<ROW1; i++) {
        for(j=0; j<COL2; j++) {
            *(shm + k) = res[i][j];
            k++;
        }
    }

    // Detach from shared memory segment
    if (shmdt(shm) == -1) {
        perror("shmdt failed");
        exit(1);
    }

    return 0;
}
