#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "../lexico/lexico.h" // Para pegar o TipoToken

// Define o tipo de dado da variável na tabela
typedef enum {
    TIPO_VAR_INTEIRO,
    TIPO_VAR_REAL,
    TIPO_VAR_CARACTER,
    TIPO_VAR_DESCONHECIDO
} TipoVariavel;

// Estrutura de um símbolo (variável)
typedef struct {
    char nome[100];
    TipoVariavel tipo;
    int linha_declaracao;
    int usada; // 0 = não, 1 = sim (útil para warning de variável não usada)
} Simbolo;

// Funções públicas
void inicializar_tabela();
int adicionar_simbolo(char *nome, TipoToken tipo_token, int linha);
Simbolo* buscar_simbolo(char *nome);
void verificar_uso_variavel(char *nome, int linha_uso);

#endif