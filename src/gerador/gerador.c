#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gerador.h"

#define MAX_CODE 2000 

static Instrucao codigo[MAX_CODE];
static int pos_codigo = 0;
static int cont_reg = 0;   
static int cont_label = 0; 

void inicializar_gerador() {
    pos_codigo = 0;
    cont_reg = 0;
    cont_label = 0;
}

// Gera um novo índice de registrador temporário
int novo_registrador() {
    return ++cont_reg;
}

// Gera um novo índice de label
int novo_label() {
    return ++cont_label;
}

// Adiciona uma instrução ao array de código
void gerar_codigo(OpCode op, char* a1, char* a2, char* a3) {
    if (pos_codigo >= MAX_CODE) {
        printf("ERRO CRITICO: Limite de codigo intermediario atingido!\n");
        exit(1);
    }
    
    Instrucao *i = &codigo[pos_codigo++];
    i->op = op;
    
    // Copia argumentos com segurança (ou string vazia se for NULL)
    if (a1) strcpy(i->arg1, a1); else i->arg1[0] = '\0';
    if (a2) strcpy(i->arg2, a2); else i->arg2[0] = '\0';
    if (a3) strcpy(i->arg3, a3); else i->arg3[0] = '\0';
}

// Auxiliar para converter enum em string para impressão final
const char* op_para_string(OpCode op) {
    switch(op) {
        case CMD_LOAD: return "LOAD";
        case CMD_LOADI: return "LOADI";
        case CMD_STORE: return "STORE";
        case CMD_ADD: return "ADD";
        case CMD_SUB: return "SUB";
        case CMD_MULT: return "MUL";
        case CMD_DIV: return "DIV";
        case CMD_CMP_IGUAL: return "CMPEQ";
        case CMD_CMP_MAIOR: return "CMPGT";
        case CMD_CMP_MENOR: return "CMPLT";
        case CMD_CMP_MAIOR_IGUAL: return "CMPGE"; 
        case CMD_CMP_MENOR_IGUAL: return "CMPLE"; 
        case CMD_CMP_DIFERENTE: return "CMPNE";   
        case CMD_JMP: return "JMP";
        case CMD_JMP_FALSE: return "JMPFALSE";
        case CMD_JMP_TRUE: return "JMPTRUE";
        case CMD_LABEL: return "LABEL";
        case CMD_HALT: return "HALT";
        default: return "UNKNOWN";
    }
}

void imprimir_codigo_intermediario() {
    printf("\n=== CODIGO INTERMEDIARIO (TAC) ===\n");
    for (int i = 0; i < pos_codigo; i++) {
        Instrucao inst = codigo[i];
        
        if (inst.op == CMD_LABEL) {
            printf("%s:", inst.arg1); 
        } else {
            // Instrução normal (Ex: ADD R1, R2, R3)
            printf("%s", op_para_string(inst.op));
            if (inst.arg1[0]) printf(" %s", inst.arg1);
            if (inst.arg2[0]) printf(", %s", inst.arg2);
            if (inst.arg3[0]) printf(", %s", inst.arg3);
        }
        printf("\n");
    }
    printf("==================================\n");
}