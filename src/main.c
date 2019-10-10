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
    if (argc > X) {
        receberEntrada(argv, &tamanho, &iesimo, &saida, &numthreads);
        processar(tamanho, iesimo, saida, numthreads);
    }
    else {
        printf("Erro ao alocar vetor de dados.\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
