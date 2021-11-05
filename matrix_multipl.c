#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <time.h>

typedef struct partmult_args {
    double ***A, ***B, ***C; //A * B = C
    size_t start, end;
    size_t sz;
    pthread_t id;
} pmArgs;

double** matrix_init(size_t m) {
    double** matrix = (double**)calloc(m, sizeof(double*));
    for (size_t i = 0; i < m; i++) {
        matrix[i] = (double*)calloc(m, sizeof(double));
    }
    return matrix;
}
void matrix_generate(double*** matrix, size_t m) {
    for (size_t y = 0; y < m; y++)
        for (size_t x = 0; x < m; x++) {
            (*matrix)[y][x] = (double)(rand() % 50);
        }
}
void matrix_print(double*** matrix, size_t m) {
    for (int y = 0; y < m; y++)
        for (int x = 0; x < m; x++) {
            if (x != m - 1)
                printf("%-10.2f", (*matrix)[y][x]);
            else
                printf("%-10.2f\n", (*matrix)[y][x]);
        }
}

double** matrix_mult(double** A, double** B, size_t m) {
    double** C = matrix_init(m);
    for (int y = 0; y < m; y++)
        for (int x = 0; x < m; x++)
            for (int i = 0; i < m; i++)
                C[y][x] += A[y][i] * B[i][x];
    return C;
}

void* matrix_partmult(void* args_) {
    pmArgs* args = (pmArgs*)args_;

    for (int y = args->start; y < args->end; y++) {
        for (int x = 0; x < args->sz; x++) {
            for (int i = 0; i < args->sz; i++)
                (*(args->C))[y][x] += (*(args->A))[y][i] * (*(args->B))[i][x];
        }
    }

    return NULL;
}

double** matrix_mult_threads(double** A, double** B, size_t m) {
    double** C = matrix_init(m);

    //int parts = get_nprocs(); //number of process
    //printf("There're %d process\n", parts); 
    //!!!with 8 threads part multiplication work slower than ordinary multiplication!!!

    int parts = 3;
    pmArgs* args = (pmArgs*)calloc(parts, sizeof(pmArgs));

    int step = m / parts;
    if (m % parts != 0)
        step++;
    int next = m;

    for (int i = 0; i < parts; i++) {
        (args[i]).A = &A;
        (args[i]).B = &B;
        (args[i]).C = &C;
        (args[i]).sz = m;

        (args[i]).end = next;
        next -= step;
        (args[i]).start = next;
        if (next <= 0)
            (args[i]).start = 0;

        pthread_create(&args[i].id, NULL, matrix_partmult, (void*) &(args[i]));
        //printf("Thread %i: from %ld to %ld\n", i, (args[i]).start, (args[i]).end);

        if (next <= 0)
            break;
    }

    for(int i = 0; i < parts; i++)
        pthread_join(args[i].id, NULL);

    return C;
}

int main() {
    printf("***Mult Square Matrix(Comparison)***\n");

    for (int i = 1; i <= 100; i++) { //Tests
        srand(i);

        size_t m = 1500;
        printf("%d) SIZE: %ld\n", i, m);

        double** A = matrix_init(m); matrix_generate(&A, m);
        double** B = matrix_init(m); matrix_generate(&B, m);
        
        clock_t timer1 = clock();
        double** C1 = matrix_mult_threads(A, B, m);
        timer1 = clock() - timer1;
        printf("PartMult in %f mls\t", ((double)timer1)/ CLOCKS_PER_SEC * 1000.f);

        clock_t timer2 = clock();
        double** C2 = matrix_mult(A, B, m);
        timer2 = clock() - timer2;
        printf("Mult in %f mls\n", ((double)timer2)/ CLOCKS_PER_SEC * 1000.f);
    }

    return 0;
}