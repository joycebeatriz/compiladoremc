#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void compilar_e_executar(char *nome_arquivo);
void interpretar_codigo();
int contem_funcao_main(const char *nome_arquivo);
int verificar_blocos(const char *nome_arquivo);
int verificar_sintaxe(const char *nome_arquivo);

int main() {
    char comando[256];
    printf("Compilador C\n\n");
    printf("Comandos:\n'compilar <nomedoarquivo.c>' para compilar e executar\n'editar <nomedoarquivo.c>' para editar um arquivo\n'interpretar' para entrar no modo de interpretação\n'sair' para sair\n");
    printf("\nDigite um comando: ");

    while (1) {
        fgets(comando, sizeof(comando), stdin);
        comando[strcspn(comando, "\n")] = 0;
        if (strncmp(comando, "compilar ", 9) == 0) {
            compilar_e_executar(comando + 9);
        } else if (strcmp(comando, "interpretar") == 0) {
            interpretar_codigo();
        } else if (strncmp(comando, "editar ", 7) == 0) {
            char *nome_arquivo = comando + 7;
            printf("Abrindo editor de texto para editar %s...\n", nome_arquivo);
            char editar_comando[512];
            snprintf(editar_comando, sizeof(editar_comando), "notepad.exe %s", nome_arquivo);
            system(editar_comando);
        } else if (strcmp(comando, "sair") == 0) {
            break;
        } else {
            printf("Comando desconhecido: %s\n", comando);
        }
        printf("\nDigite um novo comando: ");
    }

    return 0;
}

void compilar_e_executar(char *nome_arquivo) {
    if (!contem_funcao_main(nome_arquivo)) {
        printf("Erro: o código deve conter uma função main.\n");
        return;
    }
    if (!verificar_blocos(nome_arquivo)) {
        printf("Erro: o código contém blocos não balanceados.\n");
        return;
    }
    if (!verificar_sintaxe(nome_arquivo)) {
        printf("Erro: o código contém erros de sintaxe.\n");
        return;
    }

    char compilacao[512];
    snprintf(compilacao, sizeof(compilacao), "gcc %s -o saida", nome_arquivo);
    int ret = system(compilacao);
    if (ret == 0) {
        printf("Compilação bem-sucedida.\n");

        // Executar o arquivo compilado e mostrar a saída
        printf("\n=== Saída do Programa ===\n");
        system("./saida");

        // Remover o arquivo de saída após a execução
        system("rm -f saida");
    } else {
        printf("Erro na compilação do arquivo %s\n", nome_arquivo);
    }
}

int contem_funcao_main(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        return 0;
    }

    char linha[512];
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strstr(linha, "int main") != NULL || strstr(linha, "void main") != NULL) {
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

int verificar_blocos(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        return 0;
    }

    int ch;
    int count_chaves = 0, count_colchetes = 0, count_parenteses = 0, count_aspas = 0, count_aspas_simples = 0;
    int dentro_aspas = 0, dentro_aspas_simples = 0;

    while ((ch = fgetc(arquivo)) != EOF) {
        if (ch == '"') {
            dentro_aspas = !dentro_aspas;
            count_aspas++;
        }
        if (ch == '\'') {
            dentro_aspas_simples = !dentro_aspas_simples;
            count_aspas_simples++;
        }
        if (!dentro_aspas && !dentro_aspas_simples) {
            if (ch == '{') count_chaves++;
            if (ch == '}') count_chaves--;
            if (ch == '[') count_colchetes++;
            if (ch == ']') count_colchetes--;
            if (ch == '(') count_parenteses++;
            if (ch == ')') count_parenteses--;
        }
    }

    fclose(arquivo);

    if (count_chaves != 0 || count_colchetes != 0 || count_parenteses != 0 || count_aspas % 2 != 0 || count_aspas_simples % 2 != 0) {
        return 0;
    }

    return 1;
}

int verificar_sintaxe(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        return 0;
    }

    char linha[512];
    while (fgets(linha, sizeof(linha), arquivo)) {
        char *token = strtok(linha, " \t\n");
        while (token != NULL) {
            if (strcmp(token, "#include") == 0 || strcmp(token, "main") == 0 || strcmp(token, "printf") == 0 ||
                strcmp(token, "scanf") == 0 || strcmp(token, "if") == 0 || strcmp(token, "else") == 0 ||
                strcmp(token, "for") == 0 || strcmp(token, "while") == 0 || strcmp(token, "switch") == 0 ||
                strcmp(token, "case") == 0) {
                // Verificação básica de comandos conhecidos
            } else {
                // Verificação de expressões ou outros elementos pode ser expandida aqui
            }
            token = strtok(NULL, " \t\n");
        }
    }

    fclose(arquivo);
    return 1;
}

void interpretar_codigo() {
    printf("Modo de Interpretação de Código.\nDigite o código e finalize com 'fim':\n");

    FILE *arquivo_temp = fopen("codigo_temp.c", "w");
    if (arquivo_temp == NULL) {
        printf("Erro ao criar arquivo temporário.\n");
        return;
    }

    // Le a entrada do usuário até a palavra-chave "fim"
    char linha[512];
    while (fgets(linha, sizeof(linha), stdin)) {
        if (strncmp(linha, "fim", 3) == 0) {
            break;
        }
        fputs(linha, arquivo_temp);
    }

    fclose(arquivo_temp);

    // Verifica se o código contém a função main
    if (!contem_funcao_main("codigo_temp.c")) {
        printf("Erro: o código deve conter uma função main.\n");
        remove("codigo_temp.c");
        return;
    }

    // Verifica se o código contém blocos balanceados
    if (!verificar_blocos("codigo_temp.c")) {
        printf("Erro: o código contém blocos não balanceados.\n");
        remove("codigo_temp.c");
        return;
    }

    // Verifica se o código contém erros de sintaxe
    if (!verificar_sintaxe("codigo_temp.c")) {
        printf("Erro: o código contém erros de sintaxe.\n");
        remove("codigo_temp.c");
        return;
    }

    char compilacao[512];
    snprintf(compilacao, sizeof(compilacao), "gcc codigo_temp.c -o saida_temp && ./saida_temp");
    int ret = system(compilacao);
    if (ret != 0) {
        printf("\nErro ao interpretar o código.\n");
    }

    remove("codigo_temp.c");
    remove("saida_temp");
}