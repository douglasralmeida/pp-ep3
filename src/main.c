#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void receberNumeros(unsigned long n, unsigned long p, float* V) {
    unsigned long i;
    
    for (i = 0; i < n; i++)
       if (scanf("%f", V + i) == 0)
           abort();
    for (; i < p; i++)
       V[i] = 0.0;
}

void receberEntrada(char* tipoexec, unsigned long* n) {
    if (scanf("%s", tipoexec) == 0)
        abort();
    if (scanf("%lu", n) == 0)
        abort();
}

int main(int argc, char* argv[]) {
    int j;
    int id;
    int meuid;
    int num_por_proc;
    int totalproc;
    char tipo[5];
    unsigned long tamanho;
    float* vetor;    
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meuid);
    MPI_Comm_size(MPI_COMM_WORLD, &totalproc);
    
    //O processo 0 recebe a entrada e a
    //redistribui para os processos disponíves
    float soma_parcial = 0.0;
    float num_recebido = 0.0;
    float* nums;
    if (meuid == 0) {
        receberEntrada(tipo, &tamanho);
        j = tamanho + (totalproc - tamanho % totalproc);
        num_por_proc = j / totalproc;
        MPI_Alloc_mem(j * sizeof(float), MPI_INFO_NULL, &vetor);
        receberNumeros(tamanho, j, vetor);
        for (j = 0; j < num_por_proc-1; j++) {
            soma_parcial += vetor[j];
        }
        if (num_por_proc > 1) {
            num_recebido = vetor[j];
            j++;
        }
        nums = vetor;
        for (id = 1; id < totalproc; id++) {
            //Primeiro envia a quantidade de números
            MPI_Send(&num_por_proc, sizeof(int), MPI_INT, id, 0, MPI_COMM_WORLD);

            //Depois envia os números em si
            MPI_Send(vetor + j, num_por_proc * sizeof(float), MPI_FLOAT, id, 0, MPI_COMM_WORLD);
            j += num_por_proc;
        }
    } else {
        int i;
        //Cada processo recebe a quantidade 
        MPI_Recv(&num_por_proc, sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Alloc_mem(num_por_proc * sizeof(float), MPI_INFO_NULL, &nums);
        
        //E depois os números da entrada
        MPI_Recv(nums, num_por_proc * sizeof(float), MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        soma_parcial = nums[0];
        for (i = 0; i < num_por_proc - 1; i++)
            soma_parcial += nums[i];
        num_recebido = nums[i];
    } 
    
    if (num_por_proc > 1) {
        //Cada processo par troca um operando 
        //com seu vizinho a direita
        int dest;
        int orig;
        if (meuid % 2) {
            dest = meuid - 1;
            orig = meuid - 1;
        }
        else {
            dest = meuid + 1;
            orig = meuid + 1;
        }
        if (totalproc % 2) {
            if (meuid == totalproc - 1) {
                dest = 0;
                orig = 1;
            } else if (meuid == 1) {
                dest = totalproc - 1;
                orig = 0;
            } else if (meuid == 0) {
                dest = 1;
                orig = totalproc - 1;
            }
        }
        MPI_Send(&num_recebido, sizeof(float), MPI_FLOAT, dest, 0, MPI_COMM_WORLD);
        MPI_Recv(&num_recebido, sizeof(float), MPI_FLOAT, orig, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    //printf("processo n. %d recebeu %f e %f.\n", meuid, ops[0], ops[1]);
    //fflush(stdout);
    
    //Realiza primeira operação de soma
    soma_parcial += num_recebido;
    
    //Primeira etapa da redução
    //Reduz a arvore para ter o número de
    //elementos igual a uma potência de 2
    int potenciadois;
    if (totalproc > 1) {
        potenciadois = 2 << ((int)log2(totalproc)-1);
        if (meuid >= potenciadois) {
            MPI_Send(&soma_parcial, sizeof(float), MPI_FLOAT, meuid-potenciadois, 0, MPI_COMM_WORLD);
            MPI_Free_mem(nums);
            MPI_Barrier(MPI_COMM_WORLD);            
            MPI_Finalize();
            //printf("AB %d\n", meuid);
            exit(EXIT_SUCCESS);
        }
        else if (meuid < totalproc-potenciadois) {
            MPI_Recv(&num_recebido, sizeof(float), MPI_FLOAT, meuid+potenciadois, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            soma_parcial += num_recebido;
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
                MPI_Recv(&num_recebido, sizeof(float), MPI_FLOAT, meuid + totalproc/2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            else {
                MPI_Send(&soma_parcial, sizeof(float), MPI_FLOAT, meuid - totalproc/2, 0, MPI_COMM_WORLD);
                break;
            }
            soma_parcial += num_recebido;
            totalproc /= 2;
        }
    }
    
    //Mostra o resultado e limpa a memória
    if (meuid == 0) {
        printf("%f\n", soma_parcial);
        fflush(stdout);
        MPI_Free_mem(vetor);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    printf("A %d\n", meuid);
    exit(EXIT_SUCCESS);
}
