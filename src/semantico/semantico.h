#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "../lexico/lexico.h"

typedef enum {
    TIPO_VAR_INTEIRO,
    TIPO_VAR_REAL,
    TIPO_VAR_CARACTER,
    TIPO_VAR_DESCONHECIDO
} TipoVariavel;

typedef struct {
    char nome[100];
    TipoVariavel tipo;
    int linha_declaracao;
    int usada; 
} Simbolo;

void inicializar_tabela();
int adicionar_simbolo(char *nome, TipoToken tipo_token, int linha);
Simbolo* buscar_simbolo(char *nome);
void verificar_uso_variavel(char *nome, int linha_uso);

// Função para exibir a tabela 
void imprimir_tabela_simbolos();

#endif