#include <stdio.h>
#include <string.h>
#include "sintatico.h"

void gerar_nome_saida(char *entrada, char *saida) {
    char *base = strrchr(entrada, '/');
    if (base) base++; 
    else base = entrada;

    sprintf(saida, "arvores/arvore_%s.dot", base);

    char *ext = strstr(saida, ".mlp.dot");
    if (ext) {
        strcpy(ext, ".dot"); 
    }
} 


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    char arquivo_arvore[200];
    gerar_nome_saida(argv[1], arquivo_arvore);

    printf("\n=== COMPILADOR MLP: Iniciando ===\n");
    printf("Arquivo alvo: %s\n", argv[1]);
    printf("Saida AST:    %s\n", arquivo_arvore);
    printf("----------------------------------\n");

    if (analise_sintatica(argv[1], arquivo_arvore)) {
        printf("\n[SUCESSO] O programa eh SINTATICAMENTE CORRETO!\n");
    } else {
        printf("\n[FALHA] O programa contem erros.\n");
    }
    printf("==================================\n");

    return 0;
}