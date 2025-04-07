#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double *a;
    double *b;
    double sum;
    int veclen;
} DOTDATA;

#define NUMTHRDS 4    // Número de threads
#define VECLEN   100000

DOTDATA dotstr;
pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexsum; // Variável mutex

void *dotprod(void *arg)
{
    int i, start, end, len;
    long offset;
    double mysum, *x, *y;

    offset = (long)arg;
    len    = dotstr.veclen;
    start  = offset * len;
    end    = start + len;
    x      = dotstr.a;
    y      = dotstr.b;
    mysum  = 0;

    for (i = start; i < end; i++) {
        mysum += (x[i] * y[i]);
    }

    pthread_mutex_lock(&mutexsum); // Executa o travamento da variável a ser manipulada
    dotstr.sum += mysum;
    printf("Thread %ld did %d to %d: mysum=%f global sum=%f\n",
           offset, start, end, mysum, dotstr.sum);
    pthread_mutex_unlock(&mutexsum); // Destrava a variável

    pthread_exit((void*) 0);
}

int main(int argc, char *argv[])
{
    long i;
    double *a, *b;
    void *status;
    pthread_attr_t attr;

    a = (double*) malloc(NUMTHRDS * VECLEN * sizeof(double));
    b = (double*) malloc(NUMTHRDS * VECLEN * sizeof(double));

    for (i = 0; i < VECLEN * NUMTHRDS; i++) {
        a[i] = 1;
        b[i] = a[i];
    }

    dotstr.veclen = VECLEN;
    dotstr.a      = a;
    dotstr.b      = b;
    dotstr.sum    = 0;

    // attr = NULL aceita atributos default
    pthread_mutex_init(&mutexsum, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for (i = 0; i < NUMTHRDS; i++) {
        pthread_create(&callThd[i], &attr, dotprod, (void *) i);
    }
    pthread_attr_destroy(&attr);

    // Aguarda a finalização de todas as threads
    for (i = 0; i < NUMTHRDS; i++) {
        pthread_join(callThd[i], &status);
    }

    printf("Sum = %f\n", dotstr.sum);

    free(a);
    free(b);
    pthread_mutex_destroy(&mutexsum);

    pthread_exit(NULL);
}
