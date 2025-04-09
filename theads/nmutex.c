#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int balance = 10000;
int totalDeposited = 0;
int totalWithdrawn = 0;

int NUM_THREADS = 2;
int OPERATIONS_PER_THREAD = 10;

void shuffleArray(char *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void *bankOperation(void *thread_id) {
    long tid = (long)thread_id;
    char operations[OPERATIONS_PER_THREAD];
    int values[OPERATIONS_PER_THREAD];

    for (int i = 0; i < OPERATIONS_PER_THREAD / 2; i++) {
        operations[i] = '+';
        operations[i + OPERATIONS_PER_THREAD / 2] = '-';
    }

    shuffleArray(operations, OPERATIONS_PER_THREAD);

    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        values[i] = (rand() % 401) + 100;
    }

    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        usleep(rand() % 100); 

        if (operations[i] == '+') {
            int prevBalance = balance;
            balance += values[i];
            totalDeposited += values[i];
            printf("Thread %ld: Depositou +%d | Saldo Antes: %d -> Depois: %d | Total Depositado: %d\n", 
                   tid, values[i], prevBalance, balance, totalDeposited);
        } else {
            int prevBalance = balance;
            balance -= values[i];
            totalWithdrawn += values[i];
            printf("Thread %ld: Retirou -%d | Saldo Antes: %d -> Depois: %d | Total Retirado: %d\n", 
                   tid, values[i], prevBalance, balance, totalWithdrawn);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc >= 3) {
        NUM_THREADS = atoi(argv[1]);
        OPERATIONS_PER_THREAD = atoi(argv[2]);
    }


    pthread_t threads[NUM_THREADS];
    srand(time(NULL));

    for (long t = 0; t < NUM_THREADS; t++) {
        if (pthread_create(&threads[t], NULL, bankOperation, (void *)t) != 0) {
            perror("Erro ao criar thread");
            return 1;
        }
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    int expectedBalance = 10000 + totalDeposited - totalWithdrawn;
    printf("\n======== RESULTADO SEM MUTEX ========\n");
    printf("Saldo final: %d\n", balance);
    printf("Total depositado: %d\n", totalDeposited);
    printf("Total retirado: %d\n", totalWithdrawn);
    printf("Saldo esperado: %d\n", expectedBalance);

    if (balance != expectedBalance) {
        printf("INCONSISTÊNCIA DETECTADA! Saldo calculado: %d, Saldo esperado: %d\n",
               balance, expectedBalance);
    } else {
        printf("Nenhuma inconsistência detectada.\n");
    }

    return 0;
}
