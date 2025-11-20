#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sintatico.h"
#include "../lexico/lexico.h"
#include "../semantico/semantico.h"
#include "../gerador/gerador.h"

static Token token_atual;
static int nivel_profundidade = 0;

static FILE *f_dot = NULL;
static int contador_nos = 0;
static int pais[100];
static int nivel_dot = 0;

void inicializar_dot(char *nome_saida) {
    f_dot = fopen(nome_saida, "w");
    if (f_dot) {
        fprintf(f_dot, "digraph G {\n");
        fprintf(f_dot, "  node [shape=box, style=filled, color=lightblue];\n");
    } else {
        printf("ERRO: Nao foi possivel criar o arquivo de arvore '%s'.\n", nome_saida);
    }
}

void finalizar_dot() {
    if (f_dot) {
        fprintf(f_dot, "}\n");
        fclose(f_dot);
        printf("\n[VISUALIZACAO] Arquivo de arvore gerado com sucesso!\n");
    }
}

void log_dot(const char* label, const char* detalhe) {
    if (!f_dot) return;
    int meu_id = contador_nos++;
    if (detalhe && strlen(detalhe) > 0) {
        fprintf(f_dot, "  n%d [label=\"%s\\n(%s)\", color=lightyellow];\n", meu_id, label, detalhe);
    } else {
        fprintf(f_dot, "  n%d [label=\"%s\"];\n", meu_id, label);
    }
    if (nivel_dot > 0) {
        int id_pai = pais[nivel_dot - 1];
        fprintf(f_dot, "  n%d -> n%d;\n", id_pai, meu_id);
    }
    pais[nivel_dot] = meu_id;
}

// Protótipos 
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
void proc_expressao(char* res_out, TipoVariavel* tipo_out);
void proc_termo(char* res_out, TipoVariavel* tipo_out);
void proc_fator(char* res_out, TipoVariavel* tipo_out);
void proc_condicao(char* res_out);
void proc_expressao_relacional(char* res_out);

//  Auxiliares 
void gerar_temp(char* buffer) { sprintf(buffer, "R%d", novo_registrador()); }
void gerar_rotulo(char* buffer) { sprintf(buffer, "L%d", novo_label()); }
void advance() { token_atual = proximo_token(); }

void eat(TipoToken tipo_esperado) {
    if (token_atual.tipo == tipo_esperado) {
        printf("[TOKEN] %-15s | Lexema: %s\n", token_para_string(token_atual.tipo), token_atual.lexema);
        advance();
    } else {
        printf("\n>>> ERRO SINTATICO [COD. 10] (Linha %d, Coluna %d) <<<\n", token_atual.linha, token_atual.coluna);
        printf("Esperado: %s, Encontrado: %s\n", token_para_string(tipo_esperado), token_atual.lexema);
        exit(1);
    }
}

void verifica_profundidade() {
    if (nivel_profundidade > 10) {
        printf("\n>>> ERRO SINTATICO [COD. 11] (Linha %d): Profundidade maxima de aninhamento (10) excedida. <<<\n", token_atual.linha);
        exit(1);
    }
}

void verificar_compatibilidade(TipoVariavel t1, TipoVariavel t2, int linha, char* op) {
    if (t1 != t2) {
        printf("ERRO SEMANTICO [COD. 22] (Linha %d): Operacao '%s' com tipos incompativeis (%s e %s).\n", 
               linha, op, tipo_para_string(t1), tipo_para_string(t2));
        exit(1);
    }
}

//  Implementação 

void proc_programa() {
    log_dot("PROGRAMA", "");
    nivel_dot++;
    
    eat(TOKEN_PROGRAMA);
    proc_declaracoes();
    proc_comandos();
    eat(TOKEN_FIM_PROG);
    gerar_codigo(CMD_HALT, NULL, NULL, NULL);
    
    nivel_dot--;
}

void proc_declaracoes() {
    if (token_atual.tipo == TOKEN_INTEIRO || token_atual.tipo == TOKEN_REAL || token_atual.tipo == TOKEN_CARACTER) {
        proc_declaracao();
        eat(TOKEN_PONTO_VIRGULA);
        proc_declaracoes();
    }
}

void proc_declaracao() {
    log_dot("DECLARACAO", "");
    nivel_dot++;
    
    TipoToken tipo_atual = token_atual.tipo;
    proc_tipo();
    
    if (token_atual.tipo == TOKEN_ID) {
        log_dot("ID", token_atual.lexema);
        adicionar_simbolo(token_atual.lexema, tipo_atual, token_atual.linha);
    }
    eat(TOKEN_ID);

    while (token_atual.tipo == TOKEN_VIRGULA) {
        advance();
        if (token_atual.tipo == TOKEN_ID) {
            log_dot("ID", token_atual.lexema);
            adicionar_simbolo(token_atual.lexema, tipo_atual, token_atual.linha);
        }
        eat(TOKEN_ID);
    }
    nivel_dot--;
}

void proc_tipo() {
    if (token_atual.tipo == TOKEN_INTEIRO) { log_dot("TIPO", "inteiro"); eat(TOKEN_INTEIRO); }
    else if (token_atual.tipo == TOKEN_REAL) { log_dot("TIPO", "real"); eat(TOKEN_REAL); }
    else if (token_atual.tipo == TOKEN_CARACTER) { log_dot("TIPO", "caracter"); eat(TOKEN_CARACTER); }
    else { printf("ERRO: Tipo invalido na linha %d\n", token_atual.linha); exit(1); }
}

void proc_comandos() {
    if (token_atual.tipo == TOKEN_SE || token_atual.tipo == TOKEN_ENQUANTO || token_atual.tipo == TOKEN_ID) {
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
    log_dot("ATRIBUICAO", "");
    nivel_dot++;
    
    char var_destino[100], reg_resultado[100];
    TipoVariavel tipo_var, tipo_expr;

    strcpy(var_destino, token_atual.lexema);
    log_dot("DESTINO", var_destino);
    
    verificar_uso_variavel(var_destino, token_atual.linha);
    tipo_var = obter_tipo_simbolo(var_destino);

    eat(TOKEN_ID);
    eat(TOKEN_ATRIB);
    
    proc_expressao(reg_resultado, &tipo_expr);
    
    if (tipo_var != tipo_expr) {
        if (!(tipo_var == TIPO_VAR_REAL && tipo_expr == TIPO_VAR_INTEIRO)) {
             printf("ERRO SEMANTICO [COD. 22] (Linha %d): Atribuicao incompativel (%s recebe %s).\n", 
                    token_atual.linha, tipo_para_string(tipo_var), tipo_para_string(tipo_expr));
             exit(1);
        }
    }

    gerar_codigo(CMD_STORE, var_destino, reg_resultado, NULL);
    eat(TOKEN_PONTO_VIRGULA);
    nivel_dot--;
}

void proc_condicional() {
    log_dot("SE", "");
    nivel_dot++;
    nivel_profundidade++; verifica_profundidade();

    char reg_cond[100], label_else[20], label_fim[20];
    gerar_rotulo(label_else); gerar_rotulo(label_fim);

    eat(TOKEN_SE);
    eat(TOKEN_ABRE_PAR);
    
    log_dot("CONDICAO", "");
    nivel_dot++;
    proc_condicao(reg_cond);
    nivel_dot--;
    
    eat(TOKEN_FECHA_PAR);
    eat(TOKEN_ENTAO);
    
    gerar_codigo(CMD_JMP_FALSE, reg_cond, label_else, NULL);
    
    log_dot("ENTAO", "");
    nivel_dot++;
    proc_comandos(); 
    nivel_dot--;
    
    gerar_codigo(CMD_JMP, label_fim, NULL, NULL);
    gerar_codigo(CMD_LABEL, label_else, NULL, NULL);
    
    if (token_atual.tipo == TOKEN_SENAO) {
        advance();
        log_dot("SENAO", "");
        nivel_dot++;
        proc_comandos();
        nivel_dot--;
    }
    
    gerar_codigo(CMD_LABEL, label_fim, NULL, NULL);
    eat(TOKEN_PONTO_VIRGULA);
    
    nivel_profundidade--;
    nivel_dot--;
}

void proc_iterativo() {
    log_dot("ENQUANTO", "");
    nivel_dot++;
    nivel_profundidade++; verifica_profundidade();
    
    char label_inicio[20], label_fim[20], reg_cond[100];
    gerar_rotulo(label_inicio); gerar_rotulo(label_fim);
    
    gerar_codigo(CMD_LABEL, label_inicio, NULL, NULL);
    
    eat(TOKEN_ENQUANTO);
    eat(TOKEN_ABRE_PAR);
    
    log_dot("CONDICAO", "");
    nivel_dot++;
    proc_condicao(reg_cond);
    nivel_dot--;
    
    eat(TOKEN_FECHA_PAR);
    
    gerar_codigo(CMD_JMP_FALSE, reg_cond, label_fim, NULL);
    
    log_dot("CORPO", "");
    nivel_dot++;
    proc_comandos();
    nivel_dot--;
    
    gerar_codigo(CMD_JMP, label_inicio, NULL, NULL);
    gerar_codigo(CMD_LABEL, label_fim, NULL, NULL);
    
    eat(TOKEN_PONTO_VIRGULA);
    
    nivel_profundidade--;
    nivel_dot--;
}

void proc_condicao(char* res_out) {
    char reg_esq[100], reg_dir[100], reg_temp[100];
    
    if (token_atual.tipo == TOKEN_NAO) {
        log_dot("OP_LOGICO", "NOT");
        printf("[TOKEN] %-15s | Lexema: %s\n", token_para_string(token_atual.tipo), token_atual.lexema);
        advance();
        
        if (token_atual.tipo == TOKEN_ABRE_PAR) {
            eat(TOKEN_ABRE_PAR);
            proc_condicao(res_out);
            eat(TOKEN_FECHA_PAR);
        } else {
            proc_expressao_relacional(res_out);
        }
        
        gerar_temp(reg_temp);
        char r_zero[20]; sprintf(r_zero, "R%d", novo_registrador());
        gerar_codigo(CMD_LOADI, r_zero, "0", NULL);
        gerar_codigo(CMD_CMP_IGUAL, reg_temp, res_out, r_zero);
        strcpy(res_out, reg_temp);
        return;
    }

    proc_expressao_relacional(reg_esq);

    while (token_atual.tipo == TOKEN_E || token_atual.tipo == TOKEN_OU) {
        TipoToken op_logico = token_atual.tipo;
        log_dot("OP_LOGICO", op_logico == TOKEN_E ? "E" : "OR");
        printf("[TOKEN] %-15s | Lexema: %s\n", token_para_string(token_atual.tipo), token_atual.lexema);
        advance();
        
        proc_expressao_relacional(reg_dir);
        
        gerar_temp(reg_temp);
        OpCode op_cmd = (op_logico == TOKEN_E) ? CMD_MULT : CMD_ADD;
        gerar_codigo(op_cmd, reg_temp, reg_esq, reg_dir);
        strcpy(reg_esq, reg_temp);
    }
    strcpy(res_out, reg_esq);
}

void proc_expressao_relacional(char* res_out) {
    char reg_esq[100], reg_dir[100];
    TipoVariavel t1, t2;
    
    if (token_atual.tipo == TOKEN_ABRE_PAR) {
        eat(TOKEN_ABRE_PAR);
        proc_condicao(res_out);
        eat(TOKEN_FECHA_PAR);
        return;
    }

    proc_expressao(reg_esq, &t1);
    
    OpCode op = CMD_HALT;
    if (token_atual.tipo == TOKEN_MAIOR) op = CMD_CMP_MAIOR;
    else if (token_atual.tipo == TOKEN_MENOR) op = CMD_CMP_MENOR;
    else if (token_atual.tipo == TOKEN_MAIOR_IGUAL) op = CMD_CMP_MAIOR_IGUAL;
    else if (token_atual.tipo == TOKEN_MENOR_IGUAL) op = CMD_CMP_MENOR_IGUAL;
    else if (token_atual.tipo == TOKEN_IGUAL_REL) op = CMD_CMP_IGUAL;
    else if (token_atual.tipo == TOKEN_DIFERENTE) op = CMD_CMP_DIFERENTE;
    
    if (op != CMD_HALT) {
        log_dot("OP_REL", token_atual.lexema);
        printf("[TOKEN] %-15s | Lexema: %s\n", token_para_string(token_atual.tipo), token_atual.lexema);
        advance();
        
        proc_expressao(reg_dir, &t2);
        verificar_compatibilidade(t1, t2, token_atual.linha, "RELACIONAL");
        
        gerar_temp(res_out);
        gerar_codigo(op, res_out, reg_esq, reg_dir);
    } else strcpy(res_out, reg_esq);
}

void proc_expressao(char* res_out, TipoVariavel* tipo_out) {
    char reg_esq[100], reg_dir[100], reg_temp[100];
    TipoVariavel t1, t2;
    
    proc_termo(reg_esq, &t1);
    
    while (token_atual.tipo == TOKEN_SOMA || token_atual.tipo == TOKEN_SUB) {
        log_dot("OP_ARIT", token_atual.lexema);
        printf("[TOKEN] %-15s | Lexema: %s\n", token_para_string(token_atual.tipo), token_atual.lexema);
        TipoToken op_token = token_atual.tipo;
        advance();
        
        proc_termo(reg_dir, &t2);
        verificar_compatibilidade(t1, t2, token_atual.linha, "SOMA/SUB");
        
        gerar_temp(reg_temp);
        gerar_codigo((op_token == TOKEN_SOMA) ? CMD_ADD : CMD_SUB, reg_temp, reg_esq, reg_dir);
        strcpy(reg_esq, reg_temp);
    }
    strcpy(res_out, reg_esq);
    *tipo_out = t1;
}

void proc_termo(char* res_out, TipoVariavel* tipo_out) {
    char reg_esq[100], reg_dir[100], reg_temp[100];
    TipoVariavel t1, t2;
    
    proc_fator(reg_esq, &t1);
    
    while (token_atual.tipo == TOKEN_MULT || token_atual.tipo == TOKEN_DIV || token_atual.tipo == TOKEN_RESTO) {
        log_dot("OP_MULT", token_atual.lexema);
        printf("[TOKEN] %-15s | Lexema: %s\n", token_para_string(token_atual.tipo), token_atual.lexema);
        TipoToken op_token = token_atual.tipo;
        advance();
        
        proc_fator(reg_dir, &t2);
        verificar_compatibilidade(t1, t2, token_atual.linha, "MULT/DIV");
        
        gerar_temp(reg_temp);
        OpCode op = CMD_MULT;
        if (op_token == TOKEN_DIV) op = CMD_DIV; 
        gerar_codigo(op, reg_temp, reg_esq, reg_dir);
        strcpy(reg_esq, reg_temp);
    }
    strcpy(res_out, reg_esq);
    *tipo_out = t1;
}

void proc_fator(char* res_out, TipoVariavel* tipo_out) {
    char reg_temp[100];
    
    if (token_atual.tipo == TOKEN_NUM_INT) {
        log_dot("NUM_INT", token_atual.lexema);
        printf("[TOKEN] %-15s | Lexema: %s\n", token_para_string(token_atual.tipo), token_atual.lexema);
        *tipo_out = TIPO_VAR_INTEIRO;
        gerar_temp(reg_temp);
        gerar_codigo(CMD_LOADI, reg_temp, token_atual.lexema, NULL);
        strcpy(res_out, reg_temp);
        advance();
    } 
    else if (token_atual.tipo == TOKEN_NUM_REAL) {
        log_dot("NUM_REAL", token_atual.lexema);
        printf("[TOKEN] %-15s | Lexema: %s\n", token_para_string(token_atual.tipo), token_atual.lexema);
        *tipo_out = TIPO_VAR_REAL;
        gerar_temp(reg_temp);
        gerar_codigo(CMD_LOADI, reg_temp, token_atual.lexema, NULL);
        strcpy(res_out, reg_temp);
        advance();
    }
    else if (token_atual.tipo == TOKEN_LITERAL_CHAR) {
        log_dot("CHAR", token_atual.lexema);
        printf("[TOKEN] %-15s | Lexema: %s\n", token_para_string(token_atual.tipo), token_atual.lexema);
        *tipo_out = TIPO_VAR_CARACTER;
        gerar_temp(reg_temp);
        gerar_codigo(CMD_LOADI, reg_temp, token_atual.lexema, NULL);
        strcpy(res_out, reg_temp);
        advance();
    }
    else if (token_atual.tipo == TOKEN_ID) {
        log_dot("VAR", token_atual.lexema);
        printf("[TOKEN] %-15s | Lexema: %s\n", token_para_string(token_atual.tipo), token_atual.lexema);
        verificar_uso_variavel(token_atual.lexema, token_atual.linha);
        *tipo_out = obter_tipo_simbolo(token_atual.lexema);
        gerar_temp(reg_temp);
        gerar_codigo(CMD_LOAD, reg_temp, token_atual.lexema, NULL);
        strcpy(res_out, reg_temp);
        advance();
    } 
    else if (token_atual.tipo == TOKEN_ABRE_PAR) {
        eat(TOKEN_ABRE_PAR);
        proc_expressao(res_out, tipo_out);
        eat(TOKEN_FECHA_PAR);
    } else {
        printf("ERRO SINTATICO [COD. 10] (Linha %d): Fator invalido.\n", token_atual.linha);
        exit(1);
    }
}

int analise_sintatica(char *nome_arquivo, char *nome_saida_dot) {
    inicializar_gerador();
    inicializar_tabela();
    
    inicializar_dot(nome_saida_dot); 
    
    if (!inicializar_lexico(nome_arquivo)) return 0;
    advance();
    proc_programa();
    if (token_atual.tipo != TOKEN_EOF && token_atual.tipo != TOKEN_FIM_PROG) 
        printf("AVISO: Codigo apos o fim do programa ignorado.\n");
    
    finalizar_lexico();
    finalizar_dot();
    
    imprimir_tabela_simbolos();
    imprimir_codigo_intermediario();
    return 1;
}