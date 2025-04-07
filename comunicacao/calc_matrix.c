#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>

typedef struct {
    int rows;
    int cols;
    double *data; 
} Matrix;

Matrix create_matrix(int rows, int cols) {
    Matrix m;
    m.rows = rows;
    m.cols = cols;
    m.data = (double *)malloc(rows * cols * sizeof(double));
    return m;
}

void free_matrix(Matrix m) {
    free(m.data);
}

void print_matrix(Matrix m) {
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            printf("%8.2f ", m.data[i * m.cols + j]);
        }
        printf("\n");
    }
}

Matrix multiply(Matrix a, Matrix b) {
    Matrix result = create_matrix(a.rows, b.cols);
    if (a.cols != b.rows) {
        result.rows = -1; 
        return result;
    }

    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < b.cols; j++) {
            double sum = 0.0;
            for (int k = 0; k < a.cols; k++) {
                sum += a.data[i * a.cols + k] * b.data[k * b.cols + j];
            }
            result.data[i * result.cols + j] = sum;
        }
    }
    return result;
}

Matrix inverse(Matrix m) {
    Matrix inv = create_matrix(m.rows, m.cols);
    if (m.rows != m.cols) {
        inv.rows = -1;
        return inv;
    }

    int n = m.rows;
    Matrix aug = create_matrix(n, 2 * n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            aug.data[i * aug.cols + j] = m.data[i * m.cols + j];
        }
        for (int j = n; j < 2 * n; j++) {
            aug.data[i * aug.cols + j] = (j - n == i) ? 1.0 : 0.0;
        }
    }

    // Eliminação de Gauss-Jordan
    for (int i = 0; i < n; i++) {
        int max_row = i;
        double max_val = fabs(aug.data[i * aug.cols + i]);
        for (int k = i + 1; k < n; k++) {
            if (fabs(aug.data[k * aug.cols + i]) > max_val) {
                max_val = fabs(aug.data[k * aug.cols + i]);
                max_row = k;
            }
        }

        if (max_row != i) {
            for (int k = 0; k < 2 * n; k++) {
                double temp = aug.data[i * aug.cols + k];
                aug.data[i * aug.cols + k] = aug.data[max_row * aug.cols + k];
                aug.data[max_row * aug.cols + k] = temp;
            }
        }

        if (fabs(aug.data[i * aug.cols + i]) < 1e-10) {
            free_matrix(aug);
            inv.rows = -2; 
            return inv;
        }

        double pivot = aug.data[i * aug.cols + i];
        for (int j = i; j < 2 * n; j++) {
            aug.data[i * aug.cols + j] /= pivot;
        }

        for (int k = 0; k < n; k++) {
            if (k != i) {
                double factor = aug.data[k * aug.cols + i];
                for (int j = i; j < 2 * n; j++) {
                    aug.data[k * aug.cols + j] -= factor * aug.data[i * aug.cols + j];
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inv.data[i * n + j] = aug.data[i * aug.cols + (j + n)];
        }
    }

    free_matrix(aug);
    return inv;
}

void send_matrix(int fd, Matrix m) {
    write(fd, &m.rows, sizeof(int));
    write(fd, &m.cols, sizeof(int));
    write(fd, m.data, m.rows * m.cols * sizeof(double));
}

Matrix receive_matrix(int fd) {
    Matrix m;
    read(fd, &m.rows, sizeof(int));
    read(fd, &m.cols, sizeof(int));
    m.data = (double *)malloc(m.rows * m.cols * sizeof(double));
    read(fd, m.data, m.rows * m.cols * sizeof(double));
    return m;
}

int main() {
    int fd[2];
    pipe(fd); 

    pid_t pid = fork();

    if (pid == 0) { 
        close(fd[0]); 

        
        Matrix B = create_matrix(2, 2);
        B.data[0] = 6; B.data[1] = 4;
        B.data[2] = 2; B.data[3] = 2;

        Matrix B_inv = inverse(B);
        printf("\nMatriz B_inv:\n");
        print_matrix(B_inv);
        if (B_inv.rows < 0) {
            fprintf(stderr, "Filho: Erro na inversão de B!\n");
            exit(1);
        }

        send_matrix(fd[1], B_inv); 
        close(fd[1]); 

        free_matrix(B);
        free_matrix(B_inv);
        exit(0);
    } 
    else { 
        close(fd[1]); 

        Matrix A = create_matrix(2, 2);
        A.data[0] = 4; A.data[1] = 7;
        A.data[2] = 2; A.data[3] = 6;

        Matrix A_inv = inverse(A);
        printf("\nMatriz A_inv:\n");
        print_matrix(A_inv);
        if (A_inv.rows < 0) {
            fprintf(stderr, "Pai: Erro na inversão de A!\n");
            exit(1);
        }

        wait(NULL);
        Matrix B_inv = receive_matrix(fd[0]);
        close(fd[0]);

        Matrix R = multiply(A_inv, B_inv);
        if (R.rows < 0) {
            fprintf(stderr, "Erro na multiplicação!\n");
            exit(1);
        }

        printf("\nMatriz resultante R:\n");
        print_matrix(R);

        free_matrix(A);
        free_matrix(A_inv);
        free_matrix(B_inv);
        free_matrix(R);
    }

    return 0;
}