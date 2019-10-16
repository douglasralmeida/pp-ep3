#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void receberNumeros(int n, int p, float* V) {
    int i;
    
    for (i = 0; i < n; i++)
       if (scanf("%f", V + i) == 0)
           abort();
    for (; i < p; i++)
       V[i] = 0.0;
}

void receberEntrada(int* saida, int* tempo, int* n) {
    char tipo[5]
    ;
    if (scanf("%s", tipo) == 0)
        abort();
        
    if (tipo[0] == 'a')
            *saida = *tempo = 1;
    else if (tipo[0] == 't')
            *tempo = 1;
    else if (tipo[0] == 's')
            *saida = 1;
    if (scanf("%d", n) == 0)
        abort();
}

void debuginfo(int p) {
    pid_t pid;
    
    pid = getpid();
    printf("Processo %d, id %d.\n", p, pid);
    fflush(stdout);
}

int main() {
    int i = 0, j;
    int meuid;
    int num_por_proc = 0;
    int totalproc = 0;
    int imprimir_saida = 0;
    int imprimir_tempo = 0;
    int tamanho;

    double tempo_inicial = 0, tempo_final;
    float soma_parcial = 0.0;
    float num_recebido = 0.0;
    float* nums = NULL;
    
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &meuid);
    MPI_Comm_size(MPI_COMM_WORLD, &totalproc);
    
    //debuginfo(meuid);
    
    //O processo 0 recebe a entrada
    float* vetor = NULL;
    if (meuid == 0) {
        receberEntrada(&imprimir_saida, &imprimir_tempo, &tamanho);
        j = tamanho + (totalproc - tamanho % totalproc);
        num_por_proc = j / totalproc;
        vetor = (float*)malloc(j * sizeof(float));
        receberNumeros(tamanho, j, vetor);
        tempo_inicial = MPI_Wtime();
    }
    
    //O processo 0 distribui a quantidade
    //de números da entrada para os demais
    if (meuid == 0)
        for (i = 1; i < totalproc; i++)
            MPI_Send(&num_por_proc, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    else
        MPI_Recv(&num_por_proc, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    //Cada processo aloca um vetor e recebe
    //sua parcela de números
    nums = (float*)calloc(num_por_proc, sizeof(float));
    if (meuid == 0) {
        for (i = 0; i < num_por_proc; i++)
            nums[i] = vetor[i];
        for (j = 1; j < totalproc; j++) {
            MPI_Send(vetor + i, num_por_proc, MPI_FLOAT, j, 0, MPI_COMM_WORLD);
            i += num_por_proc;
        }
    } else {
        MPI_Recv(nums, num_por_proc, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    //Limpa vetor de entrada
    if (meuid == 0) {
        free(vetor);
        vetor = NULL;
    }
    
    //Cada processo irá trocar um operando com seu vizinho para somar.
    //pares vão para direita
    //impares vão para esquerda
    soma_parcial = nums[0];
    if (num_por_proc > 1) {
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
        for (i = 1; i < num_por_proc; i++) {
            MPI_Send(nums + i, 1, MPI_FLOAT, dest, 0, MPI_COMM_WORLD);
            MPI_Recv(&num_recebido, 1, MPI_FLOAT, orig, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            soma_parcial += num_recebido;
        }
    }
    
    //Primeira etapa da redução
    //Reduz a arvore para ter o número de
    //elementos igual a uma potência de 2
    int potenciadois;
    int possuitarefas = 1;
    potenciadois = 2 << ((int)log2(totalproc)-1);
    if (meuid >= potenciadois) {
        int dest = meuid - potenciadois;
        MPI_Send(&soma_parcial, 1, MPI_FLOAT, dest, 0, MPI_COMM_WORLD);
        possuitarefas = 0;
    }
    else if (meuid < totalproc-potenciadois) {
        int orig = meuid + potenciadois;
        MPI_Recv(&num_recebido, 1, MPI_FLOAT, orig, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        soma_parcial += num_recebido;
    }
    totalproc = potenciadois;
   
    //Segunda etapa da reducação
    //Reduz a arvore até sobrar um único
    //elemento
    while (possuitarefas) {
        int dest;
        int orig;
        if (totalproc == 1) {
            possuitarefas = 0;
            break;
        }

        //Distribui os resultados para outros processos
        //0 a n/2-1 recebe, n/2 a n-1 envia
        if (meuid < totalproc / 2) {
            dest = meuid + totalproc / 2;
            MPI_Recv(&num_recebido, 1, MPI_FLOAT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            soma_parcial += num_recebido;
            totalproc = totalproc / 2;
        }
        else {
            orig = meuid - totalproc / 2;
            MPI_Send(&soma_parcial, 1, MPI_FLOAT, orig, 0, MPI_COMM_WORLD);
            possuitarefas = 0;
            break;
        }
    }    
    //Mostra o resultado e limpa a memória
    if (meuid == 0) {
        tempo_final = MPI_Wtime();
        if (imprimir_saida) {
            printf("%.2lf\n", soma_parcial);
            fflush(stdout);
        }
        if (imprimir_tempo) {
            printf("%.2lf\n", (tempo_final - tempo_inicial) * 1000.0);
            fflush(stdout);
        }
    }
    //Limpa memória e finaliza
    MPI_Barrier(MPI_COMM_WORLD);
    free(nums);
    nums = NULL;
    MPI_Finalize();
    exit(EXIT_SUCCESS);
}
