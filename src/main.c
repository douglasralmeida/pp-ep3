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
    int meuid;
    int totalproc;
       
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meuid);
    MPI_Comm_size(MPI_COMM_WORLD, &totalproc);
    
    printf("teste do proc n. %d de %d.\n", meuid, totalproc);	
    MPI_Finalize();
    exit(EXIT_SUCCESS);
}
