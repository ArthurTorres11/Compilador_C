#ifndef GERADOR_H
#define GERADOR_H

// Mnemônicos definidos na Tabela do PDF (Pág 5)
typedef enum {
    CMD_LOAD,      // Carrega valor da memória p/ registrador
    CMD_LOADI,     // Carrega imediato (constante) p/ registrador
    CMD_STORE,     // Guarda registrador na memória
    
    CMD_ADD,       // Soma
    CMD_SUB,       // Subtração
    CMD_MULT,      // Multiplicação
    CMD_DIV,       // Divisão
    
    CMD_CMP_IGUAL, // == (CMPEQ)
    CMD_CMP_MAIOR, // >  (CMPGT)
    CMD_CMP_MENOR, // <  (CMPLT)
    CMD_CMP_MAIOR_IGUAL, // >= (Adaptação CMPGE)
    CMD_CMP_MENOR_IGUAL, // <= (Adaptação CMPLE)
    CMD_CMP_DIFERENTE,   // != (Adaptação CMPNE)

    CMD_JMP,       // Salto incondicional
    CMD_JMP_FALSE, // Salto se falso
    CMD_JMP_TRUE,  // Salto se verdadeiro
    
    CMD_LABEL,     // Marcador de posição (L1, L2...)
    CMD_HALT       // Fim do programa (Stop)
} OpCode;

// Estrutura de uma instrução de 3 endereços
// Ex: ADD R1, R2, R3  -> op=ADD, arg1=R1, arg2=R2, arg3=R3
typedef struct {
    OpCode op;
    char arg1[50];
    char arg2[50];
    char arg3[50];
} Instrucao;

// Funções públicas
void inicializar_gerador();
void gerar_codigo(OpCode op, char* a1, char* a2, char* a3); 
int novo_registrador(); // Retorna o número de um novo reg temporário (ex: 1, 2...)
int novo_label();       // Retorna o número de um novo label (ex: 1, 2...)
void imprimir_codigo_intermediario();

#endif