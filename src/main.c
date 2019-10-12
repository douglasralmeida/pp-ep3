#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void receberEntrada(char* tipoexec, unsigned long* n, float* V[]) {
    unsigned long i;
    
    scanf("%s", tipoexec);
    scanf("%lu", n);
    *V = (float*)malloc(*n * sizeof(float));
    for (i = 0; i < *n; i++)
       scanf("%f", *V + i);
}

int main(int argc, char* argv[]) {
    int j;
    int id;
    int meuid;
    int totalproc;
    char tipo[5];
    unsigned long tamanho;
    float* vetor;
    

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meuid);
    MPI_Comm_size(MPI_COMM_WORLD, &totalproc);
    
    //O processo 0 recebe a entrada e a
    //redistribui para os processos disponíves
    float ops[2];
    if (meuid == 0) {
        receberEntrada(tipo, &tamanho, &vetor);
        printf("\n");
        fflush(stdout);
        j = 2;
        for (id = 1; id < totalproc; id++) {
            MPI_Send(vetor + j, 2 * sizeof(float), MPI_FLOAT, id, 0, MPI_COMM_WORLD);
            j += 2;
        }
        ops[0] = vetor[0];
        ops[1] = vetor[1];
    } else {
        //Cada processo recebe sua entrada
        MPI_Recv(ops, 2 * sizeof(float), MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    //printf("processo n. %d recebeu %f e %f.\n", meuid, ops[0], ops[1]);
    //fflush(stdout); 
    
    //Cada processo troca um operando com seu vizinho
    //a esquerda
    int orig;
    int dest;
    if (meuid % 2)
        dest = meuid - 1;
    else
        dest = meuid + 1;
    orig = dest;
    
    //se o número de processos é impar,
    //o processo n-1 manda para 0, 0 para 1 e
    //1 envia para n-1
    if (totalproc % 2) {
        if (meuid == 0) {
            orig = totalproc - 1;
            dest = 1;
        }
        if (meuid == 1) {
            dest =  totalproc - 1;
            orig = 0;
        }
        else if (meuid == totalproc - 1) {
            dest = 0;
            orig = 1;
        }
    }
    MPI_Send(ops+1, sizeof(float), MPI_FLOAT, dest, 0, MPI_COMM_WORLD);
    MPI_Recv(ops+1, sizeof(float), MPI_FLOAT, orig, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    printf("processo n. %d recebeu %f e %f.\n", meuid, ops[0], ops[1]);
    fflush(stdout);
    
    //Começa a realizar as operações de soma

    MPI_Finalize();
    exit(EXIT_SUCCESS);
}
