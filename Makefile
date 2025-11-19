# Nome do executável
TARGET = compilador
CC = gcc


INCLUDES = -Isrc -Isrc/lexico -Isrc/sintatico -Isrc/semantico -Isrc/gerador

# Flags
CFLAGS = -Wall -Wextra -g $(INCLUDES)

# Diretório de build
BUILD_DIR = build

# --- DEFINIÇÃO DOS ARQUIVOS FONTE (ABORDAGEM SEGURA) ---
# 1. Pega o main.c na raiz do src
SRCS_ROOT = $(wildcard src/*.c)
# 2. Pega todos os .c dentro das subpastas (lexico, sintatico, semantico)
SRCS_SUB = $(wildcard src/*/*.c)

# Junta tudo
SRCS = $(SRCS_ROOT) $(SRCS_SUB)

# Cria a lista de objetos (.o) mantendo a estrutura de pastas
# Ex: src/main.c vira build/src/main.o
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

# Regra Principal
all: $(TARGET)

# Linkagem
$(TARGET): $(OBJS)
	@echo "Linkando executável..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $(BUILD_DIR)/$(TARGET)
	@echo "✅ Sucesso! Executável em: $(BUILD_DIR)/$(TARGET)"

# Compilação Genérica (Funciona para qualquer .c dentro de src ou subpastas)
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -rf $(BUILD_DIR)

# Rodar (ajuste o nome do arquivo de teste conforme sua necessidade)
run: all
	@echo "--- Executando ---"
	./$(BUILD_DIR)/$(TARGET) testes/erro_semantico.mlp