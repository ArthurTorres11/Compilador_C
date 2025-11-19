#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexico.h"

// Variáveis globais internas
static FILE *arquivo = NULL;
static int linha_atual = 1;
static int coluna_atual = 0;
static int prox_char = ' '; 

// Verifica se o arquivo abriu corretamente
int inicializar_lexico(char *nome_arquivo) {
    arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro: Nao foi possivel abrir o arquivo %s\n", nome_arquivo);
        return 0;
    }
    linha_atual = 1;
    coluna_atual = 0;
    prox_char = fgetc(arquivo);
    return 1;
}

void finalizar_lexico() {
    if (arquivo) fclose(arquivo);
}

static int ler_char() {
    int c = prox_char;
    if (c == '\n') {
        linha_atual++;
        coluna_atual = 0;
    } else {
        coluna_atual++;
    }
    prox_char = fgetc(arquivo);
    return c;
}

Token proximo_token() {
    Token t;
    t.lexema[0] = '\0';
    t.tipo = TOKEN_EOF;
    
    while (isspace(prox_char)) {
        ler_char();
    }

    t.linha = linha_atual;
    t.coluna = coluna_atual + 1;

    if (prox_char == EOF) {
        t.tipo = TOKEN_EOF;
        strcpy(t.lexema, "EOF");
        return t;
    }

    if (isalpha(prox_char)) {
        int i = 0;
        while (isalnum(prox_char)) {
            if (i < 99) t.lexema[i++] = (char)ler_char();
            else ler_char();
        }
        t.lexema[i] = '\0';

        if (strlen(t.lexema) > 10) {
            printf("ERRO LEXICO (Linha %d, Col %d): Identificador '%s' excede 10 caracteres.\n", t.linha, t.coluna, t.lexema);
            t.tipo = TOKEN_ERRO;
            return t;
        }

        if (strcmp(t.lexema, "inteiro") == 0) t.tipo = TOKEN_INTEIRO;
        else if (strcmp(t.lexema, "real") == 0) t.tipo = TOKEN_REAL;
        else if (strcmp(t.lexema, "caracter") == 0) t.tipo = TOKEN_CARACTER;
        else if (strcmp(t.lexema, "se") == 0) t.tipo = TOKEN_SE;
        else if (strcmp(t.lexema, "entao") == 0) t.tipo = TOKEN_ENTAO;
        else if (strcmp(t.lexema, "senao") == 0) t.tipo = TOKEN_SENAO;
        else if (strcmp(t.lexema, "enquanto") == 0) t.tipo = TOKEN_ENQUANTO;
        else if (strcmp(t.lexema, "E") == 0) t.tipo = TOKEN_E;
        else if (strcmp(t.lexema, "OR") == 0) t.tipo = TOKEN_OU;
        else if (strcmp(t.lexema, "NOT") == 0) t.tipo = TOKEN_NAO;
        else if (strcmp(t.lexema, "RESTO") == 0) t.tipo = TOKEN_RESTO;
        else t.tipo = TOKEN_ID;

        return t;
    }

    if (isdigit(prox_char)) {
        int i = 0;
        while (isdigit(prox_char)) {
            t.lexema[i++] = (char)ler_char();
        }
        if (prox_char == '.') {
            t.lexema[i++] = (char)ler_char();
            while (isdigit(prox_char)) {
                t.lexema[i++] = (char)ler_char();
            }
            t.tipo = TOKEN_NUM_REAL;
        } else {
            t.tipo = TOKEN_NUM_INT;
        }
        t.lexema[i] = '\0';
        return t;
    }

    char c = (char)ler_char();
    t.lexema[0] = c;
    t.lexema[1] = '\0';

    switch (c) {
        case '+': t.tipo = TOKEN_SOMA; break;
        case '-': t.tipo = TOKEN_SUB; break;
        case '*': t.tipo = TOKEN_MULT; break;
        case '/': t.tipo = TOKEN_DIV; break;
        case '(': t.tipo = TOKEN_ABRE_PAR; break;
        case ')': t.tipo = TOKEN_FECHA_PAR; break;
        case ';': t.tipo = TOKEN_PONTO_VIRGULA; break;
        case ',': t.tipo = TOKEN_VIRGULA; break;
        
        case '$': 
            if (prox_char == '.') {
                ler_char();
                strcat(t.lexema, ".");
                t.tipo = TOKEN_FIM_PROG;
            } else {
                t.tipo = TOKEN_PROGRAMA;
            }
            break;

        case '>':
            if (prox_char == '=') {
                ler_char();
                strcat(t.lexema, "=");
                t.tipo = TOKEN_MAIOR_IGUAL;
            } else {
                t.tipo = TOKEN_MAIOR;
            }
            break;

        case '<':
            if (prox_char == '=') {
                ler_char();
                strcat(t.lexema, "=");
                t.tipo = TOKEN_MENOR_IGUAL;
            } else {
                t.tipo = TOKEN_MENOR;
            }
            break;

        case '!':
            if (prox_char == '=') {
                ler_char();
                strcat(t.lexema, "=");
                t.tipo = TOKEN_DIFERENTE;
            } else {
                t.tipo = TOKEN_ERRO;
            }
            break;

        case '=':
            if (prox_char == '=') {
                ler_char();
                strcat(t.lexema, "=");
                t.tipo = TOKEN_IGUAL_REL;
            } else {
                t.tipo = TOKEN_ATRIB;
            }
            break;
        
        case '\'':
             t.tipo = TOKEN_LITERAL_CHAR;
             t.lexema[1] = (char)ler_char();
             if (prox_char == '\'') ler_char();
             t.lexema[2] = '\0'; 
             break;

        default:
            t.tipo = TOKEN_ERRO;
            break;
    }

    return t;
}

const char* token_para_string(TipoToken tipo) {
    switch (tipo) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERRO: return "ERRO";
        case TOKEN_PROGRAMA: return "PROGRAMA_INICIO";
        case TOKEN_FIM_PROG: return "PROGRAMA_FIM";
        case TOKEN_INTEIRO: return "INTEIRO";
        case TOKEN_REAL: return "REAL";
        case TOKEN_CARACTER: return "CARACTER";
        case TOKEN_SE: return "SE";
        case TOKEN_ENTAO: return "ENTAO";
        case TOKEN_SENAO: return "SENAO";
        case TOKEN_ENQUANTO: return "ENQUANTO";
        
        case TOKEN_ID: return "ID";
        case TOKEN_NUM_INT: return "NUM_INT";
        case TOKEN_NUM_REAL: return "NUM_REAL";
        case TOKEN_LITERAL_CHAR: return "LITERAL_CHAR";

        case TOKEN_SOMA: return "SOMA";
        case TOKEN_SUB: return "SUB";
        case TOKEN_MULT: return "MULT";
        case TOKEN_DIV: return "DIV";
        case TOKEN_RESTO: return "RESTO";
        case TOKEN_ATRIB: return "ATRIBUICAO";
        
        case TOKEN_MAIOR: return "MAIOR";
        case TOKEN_MENOR: return "MENOR";
        case TOKEN_MAIOR_IGUAL: return "MAIOR_IGUAL";
        case TOKEN_MENOR_IGUAL: return "MENOR_IGUAL";
        case TOKEN_IGUAL_REL: return "IGUAL_REL";
        case TOKEN_DIFERENTE: return "DIFERENTE";
        
        case TOKEN_E: return "E_LOGICO";
        case TOKEN_OU: return "OU_LOGICO";
        case TOKEN_NAO: return "NAO_LOGICO";

        case TOKEN_ABRE_PAR: return "ABRE_PAR";
        case TOKEN_FECHA_PAR: return "FECHA_PAR";
        case TOKEN_VIRGULA: return "VIRGULA";
        case TOKEN_PONTO_VIRGULA: return "PONTO_VIRGULA";
        
        default: return "OUTRO";
    }
}