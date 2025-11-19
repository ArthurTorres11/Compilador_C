/* src/sintatico/sintatico.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sintatico.h"
#include "../lexico/lexico.h"
#include "../semantico/semantico.h"
#include "../gerador/gerador.h"

static Token token_atual;

// --- Protótipos ---
void advance();
void eat(TipoToken tipo_esperado);
void proc_programa();
void proc_declaracoes();
void proc_declaracao();
void proc_tipo();
void proc_comandos(); // Plural (Lista de comandos)
void proc_comando();  // Singular (Um comando)
void proc_atribuicao();
void proc_condicional();
void proc_iterativo();
void proc_expressao(char* res_out);
void proc_termo(char* res_out);
void proc_fator(char* res_out);
void proc_condicao(char* res_out);

// --- Auxiliares ---
void gerar_temp(char* buffer) { sprintf(buffer, "R%d", novo_registrador()); }
void gerar_rotulo(char* buffer) { sprintf(buffer, "L%d", novo_label()); }
void advance() { token_atual = proximo_token(); }

void eat(TipoToken tipo_esperado) {
    if (token_atual.tipo == tipo_esperado) {
        advance();
    } else {
        printf("\n>>> ERRO SINTATICO (Linha %d) <<<\n", token_atual.linha);
        printf("Esperado: %s, Encontrado: %s\n", token_para_string(tipo_esperado), token_atual.lexema);
        exit(1);
    }
}

// --- Gramática ---

void proc_programa() {
    eat(TOKEN_PROGRAMA);
    proc_declaracoes();
    proc_comandos();
    eat(TOKEN_FIM_PROG);
    gerar_codigo(CMD_HALT, NULL, NULL, NULL);
}

void proc_declaracoes() {
    if (token_atual.tipo == TOKEN_INTEIRO || token_atual.tipo == TOKEN_REAL || token_atual.tipo == TOKEN_CARACTER) {
        proc_declaracao();
        eat(TOKEN_PONTO_VIRGULA);
        proc_declaracoes();
    }
}

void proc_declaracao() {
    TipoToken tipo_atual = token_atual.tipo;
    proc_tipo();
    if (token_atual.tipo == TOKEN_ID) adicionar_simbolo(token_atual.lexema, tipo_atual, token_atual.linha);
    eat(TOKEN_ID);
    while (token_atual.tipo == TOKEN_VIRGULA) {
        advance();
        if (token_atual.tipo == TOKEN_ID) adicionar_simbolo(token_atual.lexema, tipo_atual, token_atual.linha);
        eat(TOKEN_ID);
    }
}

void proc_tipo() {
    if (token_atual.tipo == TOKEN_INTEIRO || token_atual.tipo == TOKEN_REAL || token_atual.tipo == TOKEN_CARACTER) advance();
    else { printf("ERRO: Tipo invalido na linha %d\n", token_atual.linha); exit(1); }
}

// --- O SEGREDO DO BLOCO ESTÁ AQUI ---
// Lê comandos enquanto houver tokens de comando (SE, ENQUANTO, ID)
// Para automaticamente se encontrar ';', 'senao' ou '$.'
void proc_comandos() {
    if (token_atual.tipo == TOKEN_SE || 
        token_atual.tipo == TOKEN_ENQUANTO || 
        token_atual.tipo == TOKEN_ID) {
        proc_comando();
        proc_comandos();
    }
}

void proc_comando() {
    if (token_atual.tipo == TOKEN_SE) proc_condicional();
    else if (token_atual.tipo == TOKEN_ENQUANTO) proc_iterativo();
    else if (token_atual.tipo == TOKEN_ID) proc_atribuicao();
    else { printf("ERRO: Comando desconhecido linha %d\n", token_atual.linha); exit(1); }
}

void proc_atribuicao() {
    char var_destino[100], reg_resultado[100];
    strcpy(var_destino, token_atual.lexema);
    verificar_uso_variavel(var_destino, token_atual.linha);
    eat(TOKEN_ID);
    eat(TOKEN_ATRIB);
    proc_expressao(reg_resultado);
    // Verifica tipo (Opcional: verificar_tipo_atribuicao(var_destino, ...))
    gerar_codigo(CMD_STORE, var_destino, reg_resultado, NULL);
    eat(TOKEN_PONTO_VIRGULA);
}

// --- ALTERADO PARA SUPORTAR BLOCOS ---
void proc_condicional() {
    char reg_cond[100], label_else[20], label_fim[20];
    gerar_rotulo(label_else);
    gerar_rotulo(label_fim);

    eat(TOKEN_SE);
    eat(TOKEN_ABRE_PAR);
    proc_condicao(reg_cond);
    eat(TOKEN_FECHA_PAR);
    eat(TOKEN_ENTAO);
    
    gerar_codigo(CMD_JMP_FALSE, reg_cond, label_else, NULL);
    
    proc_comandos(); // <--- AGORA ACEITA MÚLTIPLOS COMANDOS!
    
    gerar_codigo(CMD_JMP, label_fim, NULL, NULL);
    gerar_codigo(CMD_LABEL, label_else, NULL, NULL);
    
    if (token_atual.tipo == TOKEN_SENAO) {
        advance();
        proc_comandos(); // <--- ELSE TAMBÉM ACEITA BLOCO!
    }
    
    gerar_codigo(CMD_LABEL, label_fim, NULL, NULL);
    eat(TOKEN_PONTO_VIRGULA); // Fecha o bloco IF
}

// --- ALTERADO PARA SUPORTAR BLOCOS ---
void proc_iterativo() {
    char label_inicio[20], label_fim[20], reg_cond[100];
    gerar_rotulo(label_inicio);
    gerar_rotulo(label_fim);
    
    gerar_codigo(CMD_LABEL, label_inicio, NULL, NULL);
    
    eat(TOKEN_ENQUANTO);
    eat(TOKEN_ABRE_PAR);
    proc_condicao(reg_cond);
    eat(TOKEN_FECHA_PAR);
    
    gerar_codigo(CMD_JMP_FALSE, reg_cond, label_fim, NULL);
    
    proc_comandos(); // <--- AGORA ACEITA MÚLTIPLOS COMANDOS!
    
    gerar_codigo(CMD_JMP, label_inicio, NULL, NULL);
    gerar_codigo(CMD_LABEL, label_fim, NULL, NULL);
    
    eat(TOKEN_PONTO_VIRGULA); // Fecha o bloco ENQUANTO
}

// --- Expressões (Igual ao anterior) ---
void proc_condicao(char* res_out) {
    char reg_esq[100], reg_dir[100];
    proc_expressao(reg_esq);
    OpCode op = CMD_HALT;
    if (token_atual.tipo == TOKEN_MAIOR) op = CMD_CMP_MAIOR;
    else if (token_atual.tipo == TOKEN_MENOR) op = CMD_CMP_MENOR;
    else if (token_atual.tipo == TOKEN_MAIOR_IGUAL) op = CMD_CMP_MAIOR_IGUAL;
    else if (token_atual.tipo == TOKEN_MENOR_IGUAL) op = CMD_CMP_MENOR_IGUAL;
    else if (token_atual.tipo == TOKEN_IGUAL_REL) op = CMD_CMP_IGUAL;
    else if (token_atual.tipo == TOKEN_DIFERENTE) op = CMD_CMP_DIFERENTE;
    
    if (op != CMD_HALT) {
        advance();
        proc_expressao(reg_dir);
        gerar_temp(res_out);
        gerar_codigo(op, res_out, reg_esq, reg_dir);
    } else strcpy(res_out, reg_esq);
}

void proc_expressao(char* res_out) {
    char reg_esq[100], reg_dir[100], reg_temp[100];
    proc_termo(reg_esq);
    while (token_atual.tipo == TOKEN_SOMA || token_atual.tipo == TOKEN_SUB) {
        TipoToken op_token = token_atual.tipo;
        advance();
        proc_termo(reg_dir);
        gerar_temp(reg_temp);
        gerar_codigo((op_token == TOKEN_SOMA) ? CMD_ADD : CMD_SUB, reg_temp, reg_esq, reg_dir);
        strcpy(reg_esq, reg_temp);
    }
    strcpy(res_out, reg_esq);
}

void proc_termo(char* res_out) {
    char reg_esq[100], reg_dir[100], reg_temp[100];
    proc_fator(reg_esq);
    while (token_atual.tipo == TOKEN_MULT || token_atual.tipo == TOKEN_DIV || token_atual.tipo == TOKEN_RESTO) {
        TipoToken op_token = token_atual.tipo;
        advance();
        proc_fator(reg_dir);
        gerar_temp(reg_temp);
        OpCode op = CMD_MULT;
        if (op_token == TOKEN_DIV) op = CMD_DIV; 
        gerar_codigo(op, reg_temp, reg_esq, reg_dir);
        strcpy(reg_esq, reg_temp);
    }
    strcpy(res_out, reg_esq);
}

void proc_fator(char* res_out) {
    char reg_temp[100];
    if (token_atual.tipo == TOKEN_NUM_INT || token_atual.tipo == TOKEN_NUM_REAL) {
        gerar_temp(reg_temp);
        gerar_codigo(CMD_LOADI, reg_temp, token_atual.lexema, NULL);
        strcpy(res_out, reg_temp);
        advance();
    } else if (token_atual.tipo == TOKEN_ID) {
        verificar_uso_variavel(token_atual.lexema, token_atual.linha);
        gerar_temp(reg_temp);
        gerar_codigo(CMD_LOAD, reg_temp, token_atual.lexema, NULL);
        strcpy(res_out, reg_temp);
        advance();
    } else if (token_atual.tipo == TOKEN_ABRE_PAR) {
        advance();
        proc_expressao(res_out);
        eat(TOKEN_FECHA_PAR);
    } else {
        printf("ERRO SINTATICO (Linha %d): Fator invalido.\n", token_atual.linha);
        exit(1);
    }
}

int analise_sintatica(char *nome_arquivo) {
    inicializar_gerador();
    inicializar_tabela();
    if (!inicializar_lexico(nome_arquivo)) return 0;
    advance();
    proc_programa();
    if (token_atual.tipo != TOKEN_EOF && token_atual.tipo != TOKEN_FIM_PROG) 
        printf("AVISO: Codigo apos o fim do programa ignorado.\n");
    finalizar_lexico();
    imprimir_tabela_simbolos();
    imprimir_codigo_intermediario();
    return 1;
}