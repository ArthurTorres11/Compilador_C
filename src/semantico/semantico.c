#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantico.h"

#define MAX_SIMBOLOS 100

static Simbolo tabela[MAX_SIMBOLOS];
static int qtd_simbolos = 0;

void inicializar_tabela() {
    qtd_simbolos = 0;
}

// Converte o token do léxico para o nosso tipo interno
TipoVariavel converter_tipo(TipoToken t) {
    switch(t) {
        case TOKEN_INTEIRO: return TIPO_VAR_INTEIRO;
        case TOKEN_REAL: return TIPO_VAR_REAL;
        case TOKEN_CARACTER: return TIPO_VAR_CARACTER;
        default: return TIPO_VAR_DESCONHECIDO;
    }
}

// Adiciona uma variável na tabela (Chamado na declaração)
int adicionar_simbolo(char *nome, TipoToken tipo_token, int linha) {
    // 1. Verifica se já existe (Erro semântico: redeclaração)
    if (buscar_simbolo(nome) != NULL) {
        printf("ERRO SEMANTICO (Linha %d): Variavel '%s' ja foi declarada anteriormente.\n", linha, nome);
        exit(1);
    }

    if (qtd_simbolos >= MAX_SIMBOLOS) {
        printf("ERRO: Tabela de simbolos cheia!\n");
        exit(1);
    }

    Simbolo *s = &tabela[qtd_simbolos++];
    strcpy(s->nome, nome);
    s->tipo = converter_tipo(tipo_token);
    s->linha_declaracao = linha;
    s->usada = 0;

    // Debug opcional
    // printf("DEBUG: Variavel '%s' declarada na linha %d.\n", nome, linha);
    return 1;
}

// Busca uma variável pelo nome
Simbolo* buscar_simbolo(char *nome) {
    for (int i = 0; i < qtd_simbolos; i++) {
        if (strcmp(tabela[i].nome, nome) == 0) {
            return &tabela[i];
        }
    }
    return NULL;
}

// Verifica se a variável existe (Chamado no uso)
void verificar_uso_variavel(char *nome, int linha_uso) {
    Simbolo *s = buscar_simbolo(nome);
    if (s == NULL) {
        printf("ERRO SEMANTICO (Linha %d): Variavel '%s' nao foi declarada.\n", linha_uso, nome);
        exit(1);
    }
    s->usada = 1;
}