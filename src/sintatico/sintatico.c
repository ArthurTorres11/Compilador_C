#include <stdio.h>
#include <stdlib.h>
#include "sintatico.h"
#include "lexico.h"

static Token token_atual;

// Regras da Gramática
void advance();
void eat(TipoToken tipo_esperado);
void proc_programa();
void proc_declaracoes();
void proc_declaracao();
void proc_tipo();
void proc_comandos();
void proc_comando();
void proc_atribuicao();
void proc_condicional();
void proc_iterativo();
void proc_expressao();
void proc_termo();
void proc_fator();
void proc_condicao();

//Funções Auxiliares

void advance() {
    token_atual = proximo_token();
    printf("Debug Sintatico: Processando token '%s'\n", token_atual.lexema);
}

void eat(TipoToken tipo_esperado) {
    if (token_atual.tipo == tipo_esperado) {
        advance();
    } else {
        printf("\n>>> ERRO SINTATICO (Linha %d, Coluna %d) <<<\n", 
               token_atual.linha, token_atual.coluna);
        printf("Esperado: %s\n", token_para_string(tipo_esperado));
        printf("Encontrado: %s ('%s')\n", token_para_string(token_atual.tipo), token_atual.lexema);
        
        finalizar_lexico();
        exit(1);
    }
}

//Implementação da Gramática BNF 

// Programa ::= $ declaracoes comandos $.
void proc_programa() {
    eat(TOKEN_PROGRAMA);
    proc_declaracoes();
    proc_comandos();
    eat(TOKEN_FIM_PROG);
}

void proc_declaracoes() {
    if (token_atual.tipo == TOKEN_INTEIRO || 
        token_atual.tipo == TOKEN_REAL || 
        token_atual.tipo == TOKEN_CARACTER) {
        
        proc_declaracao();
        eat(TOKEN_PONTO_VIRGULA);
        proc_declaracoes();
    }
}

void proc_declaracao() {
    proc_tipo();
    eat(TOKEN_ID);
    while (token_atual.tipo == TOKEN_VIRGULA) {
        advance();
        eat(TOKEN_ID);
    }
}

void proc_tipo() {
    if (token_atual.tipo == TOKEN_INTEIRO) advance();
    else if (token_atual.tipo == TOKEN_REAL) advance();
    else if (token_atual.tipo == TOKEN_CARACTER) advance();
    else {
        printf("ERRO SINTATICO (Linha %d): Esperado tipo válido.\n", token_atual.linha);
        exit(1);
    }
}

void proc_comandos() {
    if (token_atual.tipo == TOKEN_SE || 
        token_atual.tipo == TOKEN_ENQUANTO || 
        token_atual.tipo == TOKEN_ID) {
        
        proc_comando();
        proc_comandos();
    }
}

void proc_comando() {
    if (token_atual.tipo == TOKEN_SE) {
        proc_condicional();
    } else if (token_atual.tipo == TOKEN_ENQUANTO) {
        proc_iterativo();
    } else if (token_atual.tipo == TOKEN_ID) {
        proc_atribuicao();
    } else {
         printf("ERRO SINTATICO (Linha %d): Comando desconhecido.\n", token_atual.linha);
         exit(1);
    }
}

void proc_atribuicao() {
    eat(TOKEN_ID);
    eat(TOKEN_ATRIB);
    proc_expressao();
    eat(TOKEN_PONTO_VIRGULA);
}

void proc_condicional() {
    eat(TOKEN_SE);
    eat(TOKEN_ABRE_PAR);
    proc_condicao();
    eat(TOKEN_FECHA_PAR);
    eat(TOKEN_ENTAO);
    
    proc_comando();
    
    if (token_atual.tipo == TOKEN_SENAO) {
        advance();
        proc_comando();
    }
    
    if (token_atual.tipo == TOKEN_PONTO_VIRGULA) {
        // advance(); // Descomente se o professor exigir ; depois do bloco todo
    }
}

void proc_iterativo() {
    eat(TOKEN_ENQUANTO);
    eat(TOKEN_ABRE_PAR);
    proc_condicao();
    eat(TOKEN_FECHA_PAR);
    proc_comando();
}

//Expressões

void proc_condicao() {
    proc_expressao();
    if (token_atual.tipo == TOKEN_MAIOR || token_atual.tipo == TOKEN_MENOR ||
        token_atual.tipo == TOKEN_MAIOR_IGUAL || token_atual.tipo == TOKEN_MENOR_IGUAL ||
        token_atual.tipo == TOKEN_IGUAL_REL || token_atual.tipo == TOKEN_DIFERENTE) {
        advance();
        proc_expressao();
    }
}

void proc_expressao() {
    proc_termo();
    while (token_atual.tipo == TOKEN_SOMA || token_atual.tipo == TOKEN_SUB) {
        advance();
        proc_termo();
    }
}

void proc_termo() {
    proc_fator();
    while (token_atual.tipo == TOKEN_MULT || token_atual.tipo == TOKEN_DIV || token_atual.tipo == TOKEN_RESTO) {
        advance();
        proc_fator();
    }
}

void proc_fator() {
    if (token_atual.tipo == TOKEN_NUM_INT || token_atual.tipo == TOKEN_NUM_REAL) {
        advance();
    } else if (token_atual.tipo == TOKEN_ID) {
        advance();
    } else if (token_atual.tipo == TOKEN_ABRE_PAR) {
        advance();
        proc_expressao();
        eat(TOKEN_FECHA_PAR);
    } else {
        printf("ERRO SINTATICO (Linha %d): Fator invalido (esperado numero, id ou parenteses).\n", 
               token_atual.linha);
        exit(1);
    }
}

int analise_sintatica(char *nome_arquivo) {
    if (!inicializar_lexico(nome_arquivo)) {
        return 0;
    }
    
    // Carrega o primeiro token
    advance();
    
    // Entra na regra inicial
    proc_programa();
    
    // Verifica se sobrou algo no arquivo
    if (token_atual.tipo != TOKEN_EOF && token_atual.tipo != TOKEN_FIM_PROG) {
        printf("AVISO: Codigo apos o fim do programa foi ignorado.\n");
    }
    
    finalizar_lexico();
    return 1;
}