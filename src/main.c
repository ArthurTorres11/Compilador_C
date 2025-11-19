#include <stdio.h>
#include "sintatico.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    printf("\n=== COMPILADOR MLP: Iniciando ===\n");
    printf("Arquivo alvo: %s\n", argv[1]);
    printf("----------------------------------\n");

    if (analise_sintatica(argv[1])) {
        printf("\n[SUCESSO] O programa eh SINTATICAMENTE CORRETO!\n");
    } else {
        printf("\n[FALHA] O programa contem erros.\n");
    }
    printf("==================================\n");

    return 0;
}