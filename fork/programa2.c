#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void factorial() {
    int num = 10;
    unsigned long long factorial = 1;
    
    for(int i = 1; i <= num; ++i) {
        factorial *= i;
    }
    printf("=== Tarefa a) Fatorial de %d ===\n", num);
    printf("Resultado: %llu\n\n", factorial);
}

void fibonacci() {
    int n = 100;
    unsigned long long first = 0, second = 1, next;
    
    printf("=== Tarefa b) 100 primeiros Fibonacci ===\n");
    for(int i = 0; i < n; ++i) {
        if(i <= 1) {
            next = i;
        } else {
            next = first + second;
            first = second;
            second = next;
        }
        printf("%llu%s", next, (i == n-1) ? "\n" : " ");
    }
    printf("\n");
}

void hanoi(int n, char from, char to, char aux) {
    if(n == 1) {
        printf("Mover disco 1 de %c para %c\n", from, to);
        return;
    }
    hanoi(n-1, from, aux, to);
    printf("Mover disco %d de %c para %c\n", n, from, to);
    hanoi(n-1, aux, to, from);
}

int main() {
    printf("Processo Pai (PID: %d)\n", getpid());
    factorial();

    pid_t child_pid = fork();
    if(child_pid < 0) {
        perror("Erro ao criar filho");
        exit(1);
    } else if(child_pid == 0) {
        printf("\nProcesso Filho (PID: %d)\n", getpid());
        fibonacci();

        pid_t grandchild_pid = fork();
        if(grandchild_pid < 0) {
            perror("Erro ao criar neto");
            exit(1);
        } else if(grandchild_pid == 0) {
            printf("\nProcesso Neto (PID: %d)\n", getpid());
            printf("=== Tarefa c) Torre de Hanoi com 6 discos ===\n");
            hanoi(6, 'A', 'C', 'B');
            exit(0);
        } else {
            wait(NULL);
            exit(0);
        }
    } else {
        wait(NULL);
    }

    return 0;
}