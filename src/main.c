#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void receberEntrada(char* tipoexec, unsigned long* n, float* V[]) {
    unsigned long i;
    
    scanf("%s", tipoexec);
    scanf("%lu", n);
    if (*n % 2)
        *V = (float*)malloc(*n+1 * sizeof(float));
    else
        *V = (float*)malloc(*n * sizeof(float));
    for (i = 0; i < *n; i++)
       scanf("%f", *V + i);
    if (*n % 2) {
        (*n)++;
        (*V)[i] = 0.0;
    }
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
    
    //Cada processo par troca um operando 
    //com seu vizinho a direita
    int vizinho;
    if (meuid % 2)
        vizinho = meuid - 1;
    else
        vizinho = meuid + 1;
    if ((totalproc % 2) && (meuid < totalproc-1)) {
        MPI_Send(ops+1, sizeof(float), MPI_FLOAT, vizinho, 0, MPI_COMM_WORLD);
        MPI_Recv(ops+1, sizeof(float), MPI_FLOAT, vizinho, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    //printf("processo n. %d recebeu %f e %f.\n", meuid, ops[0], ops[1]);
    //fflush(stdout);
    
    //Realiza primeira operação de soma
    ops[0] += ops[1];
    
    //Primeira etapa da redução
    //Reduz a arvore para ter o número de
    //elementos igual a uma potência de 2
    int potenciadois;
    if (totalproc > 1) {
        potenciadois = 2 << ((int)log2(totalproc)-1);
        if (meuid >= potenciadois) {
            MPI_Send(ops, sizeof(float), MPI_FLOAT, meuid-potenciadois, 0, MPI_COMM_WORLD);
            MPI_Finalize();
            exit(EXIT_SUCCESS);
        }
        else if (meuid < totalproc-potenciadois) {
            MPI_Recv(ops+1, sizeof(float), MPI_FLOAT, meuid+potenciadois, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            ops[0] += ops[1];
        }
        totalproc = potenciadois;
            
        //Segunda etada da reducação
        //Reduz a arvore até sobrar um único
        //elemento
        while (1) {
            if (totalproc == 1)
                break;
            //Distribui os resultados para outros processos
            //0 a n/2-1 recebe, n/2 a n-1 envia
            if (meuid < totalproc / 2)
                MPI_Recv(ops+1, sizeof(float), MPI_FLOAT, meuid + totalproc/2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            else {
                MPI_Send(ops, sizeof(float), MPI_FLOAT, meuid - totalproc/2, 0, MPI_COMM_WORLD);
                break;
            }
            ops[0] += ops[1];        
            totalproc /= 2;
        }
    }
    
    //Mostra o resultado e limpa a memória
    if (meuid == 0) {
        printf("%f\n", ops[0]);
        //free(vetor);
    }

    MPI_Finalize();
    exit(EXIT_SUCCESS);
}
