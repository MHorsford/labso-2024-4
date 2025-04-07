#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Variáveis compartilhadas e mutex
int balance = 10000;
int totalDeposited = 0; // Total depositado
int totalWithdrawn = 0; // Total retirado
pthread_mutex_t balanceMutex = PTHREAD_MUTEX_INITIALIZER;

// Configurações dinâmicas
int NUM_THREADS = 2;
int OPERATIONS_PER_THREAD = 10;

// Embaralhamento Fisher-Yates
void shuffleArray(char *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// Função da thread
void *bankOperation(void *thread_id) {
    long tid = (long)thread_id;
    char operations[OPERATIONS_PER_THREAD];
    int values[OPERATIONS_PER_THREAD];

    // Preenche operações com metade '+' e metade '-'
    for (int i = 0; i < OPERATIONS_PER_THREAD / 2; i++) {
        operations[i] = '+';
        operations[i + OPERATIONS_PER_THREAD / 2] = '-';
    }

    shuffleArray(operations, OPERATIONS_PER_THREAD);

    // Gera valores aleatórios entre 100 e 500
    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        values[i] = (rand() % 401) + 100;
    }

    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        usleep(rand() % 100); 

        pthread_mutex_lock(&balanceMutex); 

        if (operations[i] == '+') {
            balance += values[i];
            totalDeposited += values[i]; 
            printf("Thread %ld: Depositou +%d. Saldo: %d\n", tid, values[i], balance);
        } else {
            balance -= values[i];
            totalWithdrawn += values[i]; 
            printf("Thread %ld: Retirou -%d. Saldo: %d\n", tid, values[i], balance);
        }

        pthread_mutex_unlock(&balanceMutex); 
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc >= 3) {
        NUM_THREADS = atoi(argv[1]);
        OPERATIONS_PER_THREAD = atoi(argv[2]);
    }

    printf("Número de threads: %d\n", NUM_THREADS);
    printf("Operações por thread: %d\n", OPERATIONS_PER_THREAD);

    pthread_t threads[NUM_THREADS];
    srand(time(NULL));

    // Cria threads
    for (long t = 0; t < NUM_THREADS; t++) {
        if (pthread_create(&threads[t], NULL, bankOperation, (void *)t) != 0) {
            perror("Erro ao criar thread");
            return 1;
        }
    }

    // Aguarda threads terminarem
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    pthread_mutex_destroy(&balanceMutex); // Boa prática

    // Verifica inconsistências
    int expectedBalance = 10000 + totalDeposited - totalWithdrawn;
    printf("\n======== RESULTADO COM MUTEX ========\n");
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