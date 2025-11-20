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

TipoVariavel converter_tipo(TipoToken t) {
    switch(t) {
        case TOKEN_INTEIRO: return TIPO_VAR_INTEIRO;
        case TOKEN_REAL: return TIPO_VAR_REAL;
        case TOKEN_CARACTER: return TIPO_VAR_CARACTER;
        default: return TIPO_VAR_DESCONHECIDO;
    }
}

const char* tipo_para_string(TipoVariavel t) {
    switch(t) {
        case TIPO_VAR_INTEIRO: return "inteiro";
        case TIPO_VAR_REAL: return "real";
        case TIPO_VAR_CARACTER: return "caracter";
        default: return "desconhecido";
    }
}

int adicionar_simbolo(char *nome, TipoToken tipo_token, int linha) {
    if (buscar_simbolo(nome) != NULL) {
        printf("ERRO SEMANTICO [COD. 21] (Linha %d): Variavel '%s' ja declarada.\n", linha, nome);
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
    return 1;
}

Simbolo* buscar_simbolo(char *nome) {
    for (int i = 0; i < qtd_simbolos; i++) {
        if (strcmp(tabela[i].nome, nome) == 0) return &tabela[i];
    }
    return NULL;
}

void verificar_uso_variavel(char *nome, int linha_uso) {
    Simbolo *s = buscar_simbolo(nome);
    if (s == NULL) {
        printf("ERRO SEMANTICO [COD. 20] (Linha %d): Variavel '%s' nao declarada.\n", linha_uso, nome);
        exit(1);
    }
    s->usada = 1;
}

TipoVariavel obter_tipo_simbolo(char *nome) {
    Simbolo *s = buscar_simbolo(nome);
    if (s) return s->tipo;
    return TIPO_VAR_DESCONHECIDO;
}

void imprimir_tabela_simbolos() {
    printf("\n=== TABELA DE SIMBOLOS ===\n");
    printf("%-10s | %-10s | %-5s | %-5s\n", "NOME", "TIPO", "LINHA", "USADA");
    printf("---------------------------------------------\n");
    for (int i = 0; i < qtd_simbolos; i++) {
        printf("%-10s | %-10s | %-5d | %-5s\n", 
               tabela[i].nome, 
               tipo_para_string(tabela[i].tipo), 
               tabela[i].linha_declaracao,
               tabela[i].usada ? "SIM" : "NAO");
    }
    printf("==========================\n");
}