/*
Travis Peck
ID: 47937062
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define SIZE 9
#define NUM_THREADS 27
// Macros are variables that can be referred to throughout the code

typedef struct {
    int row;
    int col;
} Parameters;
// Data structure containing two integers

int sudoku[SIZE][SIZE] = {
    {5, 3, 4, 6, 7, 8, 9, 1, 2},
    {6, 7, 2, 1, 9, 5, 3, 4, 8},
    {1, 9, 8, 3, 4, 2, 5, 6, 7},
    {8, 5, 9, 7, 6, 1, 4, 2, 3},
    {4, 2, 6, 8, 5, 3, 7, 9, 1},
    {7, 1, 3, 9, 2, 4, 8, 5, 6},
    {9, 6, 1, 5, 3, 7, 2, 8, 4},
    {2, 8, 7, 4, 1, 9, 6, 3, 5},
    {3, 4, 5, 2, 8, 6, 1, 7, 9}
};
// Initialized a sample solution

bool row_valid[SIZE] = {false};
bool col_valid[SIZE] = {false};
bool subgrid_valid[SIZE] = {false};
// Initializing boolean arrays to determine if different threads are valid

void* validate_row(void* param) {
    // Function to determine valid row
    // Meaning, no duplicates or 'out of bounds' numbers in the row
    Parameters* p = (Parameters*) param;
    int row = p->row;
    bool seen[SIZE +1] = {false};

    // Iterating through the row, exit if duplicate or out of bounds
    for (int col = 0; col < SIZE; col++) {
        int num = sudoku[row][col];
        if (num < 1 || num > 9 || seen[num]) {
            printf("The incorrect number is %d!\n", num);
            pthread_exit(NULL);
        }
        seen[num] = true;
    }
    // If we make it here, row is valid
    row_valid[row] = true;
    pthread_exit(NULL);
}

void* validate_col(void* param) {
    // Function to validate column
    Parameters* p = (Parameters*) param;
    int col = p->col;
    bool seen[SIZE+1] = {false};

    // Iterating through column to find duplicate or out of bounds values
    for (int row = 0; row < SIZE; row++) {
        int num = sudoku[row][col];
        if (num < 1 ||  num > 9 || seen[num]) {
            printf("The incorrect number is %d!\n", num);
            pthread_exit(NULL);
        }
        seen[num] = true;
    }
    // If we make it here, the column is valid
    col_valid[col] = true;
    pthread_exit(NULL);
}

void* validate_subgrid(void* param) {
    // Function to determine valid subgrid
    Parameters* p = (Parameters*) param;
    int startRow = p->row;
    int startCol = p->col;
    bool seen[SIZE + 1] = {false};

    // Iterating through the subgrid to find duplicates or out of bounds numbers
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int num = sudoku[startRow + i][startCol + j];
            if (num < 1 || num > 9 || seen[num]) {
                printf("The incorrect number is %d!\n", num);
                pthread_exit(NULL);
            }
            seen[num] = true;
        }
    }
    // If we get here, the subgrid is valid
    subgrid_valid[startRow / 3 * 3 + startCol / 3] = true;
    pthread_exit(NULL);
}

int main() {
    // Initializing the threads and structure
    pthread_t threads[NUM_THREADS];
    Parameters params[NUM_THREADS];
    int thread_index = 0;

    // Using the for loop to create the threads for each row and column, total of 18 threads for all rows and columns
    for (int i = 0; i < SIZE; i++) {
        params[thread_index].row = i;
        pthread_create(&threads[thread_index], NULL, validate_row, &params[thread_index]);
        thread_index++;

        params[thread_index].col = i;
        pthread_create(&threads[thread_index], NULL, validate_col, &params[thread_index]);
        thread_index++;
    }

    // Using another for loop to validate the subgrids
    // Finds the starting point for each of the subgrids, creates a thread for each. Now we have a total of 27 threads
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3) {
            params[thread_index].row = i;
            params[thread_index].col = j;
            pthread_create(&threads[thread_index], NULL, validate_subgrid, &params[thread_index]);
            thread_index++;
        }
    }

    // Joining all the threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Making sure everything is valid
    bool valid = true;
    for (int i = 0; i < SIZE; i++) {
        if (!row_valid[i] || !col_valid[i] || !subgrid_valid[i]) {
            valid = false;
            break;
        }
    }

    // Printing the result
    if (valid) {
        printf("The Sudoku solution is valid.\n");
    } else {
        printf("The Sudoku solution is invalid.\n");
    }

    return 0;
}

