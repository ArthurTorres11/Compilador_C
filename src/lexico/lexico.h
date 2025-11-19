/* src/lexico.h */
#ifndef LEXICO_H
#define LEXICO_H

#include <stdio.h>

// Enumeração de todos os tipos de tokens possíveis
typedef enum {
    TOKEN_EOF = 0,
    TOKEN_ERRO,

    // Palavras Reservadas
    TOKEN_PROGRAMA,      // Representa o inicio (f ou $)
    TOKEN_FIM_PROG,      // $.
    TOKEN_INTEIRO,       // inteiro
    TOKEN_REAL,          // real
    TOKEN_CARACTER,      // caracter
    TOKEN_SE,            // se
    TOKEN_ENTAO,         // entao
    TOKEN_SENAO,         // senao
    TOKEN_ENQUANTO,      // enquanto

    // Identificadores e Literais
    TOKEN_ID,            // Variaveis
    TOKEN_NUM_INT,       // 10
    TOKEN_NUM_REAL,      // 10.5
    TOKEN_LITERAL_CHAR,  // 'a'

    // Operadores
    TOKEN_SOMA,          // +
    TOKEN_SUB,           // -
    TOKEN_MULT,          // *
    TOKEN_DIV,           // /
    TOKEN_RESTO,         // RESTO
    TOKEN_ATRIB,         // =

    // Relacionais
    TOKEN_MAIOR,         // >
    TOKEN_MENOR,         // <
    TOKEN_MAIOR_IGUAL,   // >=
    TOKEN_MENOR_IGUAL,   // <=
    TOKEN_IGUAL_REL,     // == (Adotado para comparação)
    TOKEN_DIFERENTE,     // !=

    // Lógicos
    TOKEN_E,             // E
    TOKEN_OU,            // OR
    TOKEN_NAO,           // NOT

    // Pontuação
    TOKEN_ABRE_PAR,      // (
    TOKEN_FECHA_PAR,     // )
    TOKEN_VIRGULA,       // ,
    TOKEN_PONTO_VIRGULA  // ;

} TipoToken;

// Estrutura completa do Token
typedef struct {
    TipoToken tipo;
    char lexema[100]; // Texto original
    int linha;
    int coluna;
} Token;

// Funções publicas
int inicializar_lexico(char *nome_arquivo);
Token proximo_token();
void finalizar_lexico();
const char* token_para_string(TipoToken tipo); 

#endif