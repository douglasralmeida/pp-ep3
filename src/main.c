#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void receberEntrada(char* argv[], long* n, long* i, int* saidacompleta, long* t) {
    *n = atol(argv[1]);
    *i = atol(argv[2]);
    *saidacompleta = 0;
    if (argv[3][0] == 'a')
        *saidacompleta = 1;
    *t = atol(argv[4]);
}

int main(int argc, char *argv[]) {   
    MPI_Init(&argc,&argv);
    printf("teste\n");	
    MPI_Finalize();
    exit(EXIT_SUCCESS);
}
