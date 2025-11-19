# Nome do executável final
TARGET = compilador

# Compilador
CC = gcc

# Detecta automaticamente todas as subpastas dentro de src/ para incluir headers
# Isso permite usar #include "lexico.h" mesmo ele estando em outra pasta
SRC_DIRS = src $(wildcard src/*)
INCLUDES = $(foreach dir, $(SRC_DIRS), -I$(dir))

# Flags de compilação:
# -Wall -Wextra: Avisos de segurança
# -g: Debug
# $(INCLUDES): Adiciona todas as pastas do src no caminho de busca
CFLAGS = -Wall -Wextra -g $(INCLUDES)

# Diretório de build (objetos temporários)
BUILD_DIR = build

# Encontra TODOS os arquivos .c recursivamente dentro de src e subpastas
SRCS = $(shell find src -name '*.c')

# Gera a lista de objetos (.o) mantendo a estrutura de pastas dentro de build/
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

# Regra padrão
all: $(TARGET)

# Linkagem final
$(TARGET): $(OBJS)
	@echo "Linkando executável..."
	$(CC) $(OBJS) -o $(BUILD_DIR)/$(TARGET)
	@echo "✅ Sucesso! Executável criado em: $(BUILD_DIR)/$(TARGET)"

# Compilação de cada arquivo .c para .o
# O comando mkdir -p garante que a pasta de destino exista
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	@echo "Limpando arquivos temporários..."
	rm -rf $(BUILD_DIR)

# Executar
run: all
	@echo "--- Executando ---"
	./$(BUILD_DIR)/$(TARGET) testes/teste1.mlp