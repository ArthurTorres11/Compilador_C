Aqui está o arquivo `README.md` completo e corrigido. Você pode copiar todo o conteúdo do bloco de código abaixo e salvar como `README.md` na raiz do seu projeto.

````markdown
# Compilador MLP (Mini Linguagem Portugol)

Este projeto consiste na implementação completa de um compilador para a linguagem **MLP** (Mini Linguagem Portugol). O sistema realiza todas as etapas de compilação: Análise Léxica, Sintática (Descendente Recursiva), Semântica (Tipagem e Escopo) e Geração de Código Intermediário (TAC - Three Address Code).

Além disso, o compilador gera uma visualização gráfica da **Árvore Sintática Abstrata (AST)** utilizando a linguagem DOT (Graphviz).

---

## 🚀 1. Preparação do Ambiente (Windows + WSL)

Este projeto foi desenvolvido para rodar em ambiente Linux. Se você está no Windows, siga os passos abaixo para configurar o **WSL (Windows Subsystem for Linux)**.

### Passo 1.1: Instalar o WSL e Ubuntu
1. Abra o **PowerShell** como Administrador.
2. Digite o comando:
   ```powershell
   wsl --install
````

3.  Reinicie o computador quando solicitado.
4.  Após reiniciar, o Ubuntu abrirá automaticamente. Crie seu usuário e senha UNIX.

### Passo 1.2: Instalar Compiladores e Ferramentas

No terminal do Ubuntu (WSL), execute os comandos abaixo para instalar o `gcc`, `make` e `graphviz`:

```bash
sudo apt update
sudo apt install build-essential graphviz gdb
```

  * `build-essential`: Instala o GCC e o Make.
  * `graphviz`: Necessário para gerar as imagens da árvore sintática (.dot -\> .png).
  * `gdb`: Depurador (opcional, mas recomendado).

### Passo 1.3: Configurar o VS Code

1.  Abra o VS Code no Windows.
2.  Instale a extensão **"WSL"** (da Microsoft).
3.  Conecte-se ao WSL (clique no botão verde no canto inferior esquerdo \> "Connect to WSL").
4.  Instale as seguintes extensões **dentro do ambiente WSL** (o VS Code mostrará um botão "Install in WSL: Ubuntu"):
      * **C/C++** (Microsoft) - Para intellisense e depuração.
      * **Graphviz Preview** (EFanZh) - Para visualizar a AST diretamente no editor.

-----

## 📂 2. Estrutura do Projeto

O projeto está organizado de forma modular:

```text
/compilador-mlp
|-- Makefile             # Script de automação de compilação
|-- README.md            # Este arquivo
|-- /src
|   |-- main.c           # Ponto de entrada (Entry point)
|   |-- /lexico          # Analisador Léxico (Tokens e ERs)
|   |-- /sintatico       # Analisador Sintático (Gramática e AST)
|   |-- /semantico       # Tabela de Símbolos e Verificação de Tipos
|   |-- /gerador         # Geração de Código Intermediário (TAC)
|-- /testes              # Arquivos de código fonte .mlp para teste
|-- /arvores             # Pasta onde os arquivos .dot da AST serão salvos
|-- /build               # Binários e objetos compilados (gerado automaticamente)
```

-----

## 🛠️ 3. Compilação

Para compilar o projeto, utilize o `Makefile` incluído. Abra o terminal na raiz do projeto e execute:

1.  **Limpar compilações anteriores (Recomendado):**

    ```bash
    make clean
    ```

2.  **Compilar o projeto:**

    ```bash
    make
    ```

Se tudo correr bem, você verá a mensagem: `✅ Sucesso! Executável em: build/compilador`.

> **Nota:** Antes da primeira execução, certifique-se de criar a pasta para as árvores:
>
> ```bash
> mkdir arvores
> ```

-----

## ▶️ 4. Execução e Testes

O compilador deve ser executado passando o caminho do arquivo fonte `.mlp` como argumento.

### Sintaxe

```bash
./build/compilador <caminho_do_arquivo>
```

### Cenários de Teste (Complexidade Crescente)

Os testes abaixo (disponíveis na pasta `testes/`) cobrem todo o alfabeto da linguagem:

#### **Teste 1: Básico (Estrutura e Tipos)**

Valida declarações, atribuições simples e condicional `se/senao`.

```bash
./build/compilador testes/teste_final_1.mlp
```

#### **Teste 2: Lógica e Repetição**

Valida o laço `enquanto` e operadores lógicos (`E`, `OR`) e relacionais.

```bash
./build/compilador testes/teste_final_2.mlp
```

#### **Teste 3: Matemática e Precedência**

Valida expressões aritméticas complexas, parênteses, `RESTO` e tipos variados (`caracter`).

```bash
./build/compilador testes/teste_final_3.mlp
```

#### **Teste 4: Complexidade Máxima ("Big Boss")**

Teste de estresse com aninhamento profundo de blocos, múltiplos laços e condicionais misturados.

```bash
./build/compilador testes/teste_final_4.mlp
```

-----

## 🌲 5. Visualizando a Árvore Sintática (AST)

Sempre que o compilador roda com sucesso, ele gera um arquivo `.dot` na pasta `arvores/` com o nome correspondente ao teste (ex: `arvore_teste_final_4.dot`).

### Opção A: Visualizar no VS Code (Recomendado)

1.  Abra o arquivo `.dot` gerado no VS Code.
2.  Pressione `Ctrl + Shift + V` (ou clique no ícone de preview no canto superior direito).
3.  A árvore gráfica aparecerá na hora.

### Opção B: Gerar Imagem PNG (Terminal)

Se quiser exportar para imagem para colocar no relatório:

```bash
dot -Tpng arvores/arvore_teste_final_4.dot -o grafo.png
```

### Opção C: WebGraphviz

Copie o conteúdo do arquivo `.dot` e cole no site [WebGraphviz](http://www.webgraphviz.com/).

-----

## 📋 6. Tratamento de Erros

O compilador implementa um sistema robusto de erros com códigos padronizados e localização (Linha/Coluna):

| Código | Tipo | Descrição |
| :--- | :--- | :--- |
| **COD. 01** | Léxico | Símbolo inválido ou desconhecido. |
| **COD. 02** | Léxico | Identificador excede o tamanho máximo (10 chars). |
| **COD. 10** | Sintático | Token inesperado (Erro gramatical). |
| **COD. 11** | Sintático | Profundidade máxima de aninhamento (10) excedida. |
| **COD. 20** | Semântico | Variável não declarada. |
| **COD. 21** | Semântico | Variável redeclarada. |
| **COD. 22** | Semântico | Incompatibilidade de tipos (Ex: Atribuir Real a Inteiro). |

-----

## 📊 7. Avaliação Final do Projeto

Conforme os requisitos do projeto, foram desenvolvidos e validados **4 programas fonte** que atendem às seguintes premissas:

  * Cada programa possui no mínimo **4 comandos**.
  * O conjunto cobre **todos os símbolos terminais** (alfabeto) da MLP.

### Entregáveis Gerados por Teste:

1.  **Análise Léxica:** Extração de tokens e identificação de palavras reservadas.
2.  **Análise Sintática:** Validação estrutural e geração da **AST**.
3.  **Análise Semântica:** Validação de tipos e construção da **Tabela de Símbolos**.
4.  **Geração de Código:** Produção de código intermediário (TAC) com instruções como `LOAD`, `STORE`, `ADD`, `JMP`.

----- 

## ✒️ Autores

Projeto desenvolvido para a disciplina de Construção de Compiladores.
* **Arthur Torres** *
  * **Data:** Novembro/2025



```
```