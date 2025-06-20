#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "expressao.h"

#define MAX 512
#define MAX_StrInFixa 1024
#define PI  3.141592653589793

typedef struct {
    char *itens[MAX];
    int topo;
} Pilha;

Pilha *inicializarPilha();

Pilha *inicializarPilha() {
    Pilha *p = (Pilha *)malloc(sizeof(Pilha));
    if (p == NULL)
    {
        printf("ERRO: não foi possível alocar memória para a pilha!\n");
        exit(1);
    }
    p->topo = -1;
    return p;
}

int pilhaVazia(Pilha *p) {
    return p->topo == -1;
}

void empilha(Pilha *p, char *str) {
    if (p->topo < MAX - 1) {
        p->itens[++p->topo] = strdup(str);
    } else {
        printf("Stack overflow\n");
    }
}

char *desempilha(Pilha *p) {
    if (p->topo >= 0) {
        return p->itens[p->topo--];
    }
    printf("Stack underflow\n");
    return NULL;
}

char *topoPilha(Pilha *p) {
    if (p && p->topo >= 0) {
        return p->itens[p->topo];
    }
    return NULL;
}

void liberarPilha(Pilha *p) {
    while (!pilhaVazia(p)) {
        free(desempilha(p));
    }
    free(p);
}


int Digito(char c) {
    return c >= '0' && c <= '9';
}

int Letra(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int Espaco(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

// Operadores
int OperadorBinario(const char *token) {
    return strcmp(token, "+") == 0 || strcmp(token, "-") == 0 ||
           strcmp(token, "*") == 0 || strcmp(token, "/") == 0 ||
           strcmp(token, "%") == 0 || strcmp(token, "^") == 0;
}

int OperadorUnario(const char *token) {
    return strcmp(token, "log") == 0 || strcmp(token, "sen") == 0 ||
           strcmp(token, "cos") == 0 || strcmp(token, "tg") == 0 ||
           strcmp(token, "raiz") == 0;
}

int prioridade(const char *token) {
    if (!token || token[0] == '\0') return 0;

    switch (token[0]) {
        case '+':
        case '-': return 1;
        case '*':
        case '/':
        case '%': return 2;
        case '^': return 3;
        default: return 0;
    }
}

float aplicarOperador(float a, float b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '%': return (int)a % (int)b;
        case '^': return powf(a, b);
        default: return 0;
    }
}

float grausParaRadianos(float graus) {
    return graus * (PI / 180.0);
}

float aplicarFuncaoUnaria(const char *nome, float valor) {
    if (strcmp(nome, "log") == 0) return log10f(valor);
    if (strcmp(nome, "sen") == 0) return sinf(grausParaRadianos(valor));
    if (strcmp(nome, "cos") == 0) return cosf(grausParaRadianos(valor));
    if (strcmp(nome, "tg") == 0)  return tanf(grausParaRadianos(valor));
    if (strcmp(nome, "raiz") == 0) return sqrtf(valor);
    return 0;
}

// Conversão Pós-fixa → Infixa
char *getFormaInFixa(char *Str) {
    Pilha *pilha = inicializarPilha();
    char token[64];
    int i = 0, j = 0;

    while (1) {
        char c = Str[i];
        if (Espaco(c) || c == '\0') {
            if (j > 0) {
                token[j] = '\0';

                if (OperadorBinario(token)) {
                    char *op2 = desempilha(pilha);
                    char *op1 = desempilha(pilha);
                    char buffer[MAX_StrInFixa];
                    snprintf(buffer, sizeof(buffer), "(%s %s %s)", op1, token, op2);
                    empilha(pilha, buffer);
                    free(op1);
                    free(op2);
                } else if (OperadorUnario(token)) {
                    char *op = desempilha(pilha);
                    char buffer[MAX_StrInFixa];
                    snprintf(buffer, sizeof(buffer), "%s(%s)", token, op);
                    empilha(pilha, buffer);
                    free(op);
                } else {
                    empilha(pilha, token);
                }

                j = 0;
            }
            if (c == '\0') break;
        } else {
            token[j++] = c;
        }
        i++;
    }

    if (!pilhaVazia(pilha)) {
        char *res = desempilha(pilha);
        liberarPilha(pilha);
        return res;
    }

    liberarPilha(pilha);
    return strdup("ERRO");
}

// Conversão Infixa → Pós-fixa
char *getFormaPosFixa(char *StrInfixa) {
    Pilha *pilhaOperadores = inicializarPilha();
    char *resultado = (char *)malloc(MAX_StrInFixa);
    resultado[0] = '\0';

    char strCopia[MAX_StrInFixa];
    strcpy(strCopia, StrInfixa);

    char *token = strtok(strCopia, " ");

    while (token != NULL) {
        // Número (operando)
        if (Digito(token[0]) || (token[0] == '.' && Digito(token[1])) || 
            (token[0] == '-' && Digito(token[1]))) {
            strcat(resultado, token);
            strcat(resultado, " ");
        }
        // Função unária (log, sen, etc.) → empilhada
        else if (OperadorUnario(token)) {
            empilha(pilhaOperadores, token);
        }
        // Operador binário
        else if (OperadorBinario(token)) {
            while (!pilhaVazia(pilhaOperadores) && strcmp(topoPilha(pilhaOperadores), "(") != 0 &&
                   prioridade(topoPilha(pilhaOperadores)) >= prioridade(token)) {
                char *op = desempilha(pilhaOperadores);
                strcat(resultado, op);
                strcat(resultado, " ");
                free(op);
            }
            empilha(pilhaOperadores, token);
        }
        // Parêntese esquerdo
        else if (strcmp(token, "(") == 0) {
            empilha(pilhaOperadores, token);
        }
        // Parêntese direito
        else if (strcmp(token, ")") == 0) {
            while (!pilhaVazia(pilhaOperadores) && strcmp(topoPilha(pilhaOperadores), "(") != 0) {
                char *op = desempilha(pilhaOperadores);
                strcat(resultado, op);
                strcat(resultado, " ");
                free(op);
            }

            // Descarta o "("
            if (!pilhaVazia(pilhaOperadores)) {
                free(desempilha(pilhaOperadores));
            }

            // Se o topo agora for uma função unária, ela deve vir após a subexpressão
            if (!pilhaVazia(pilhaOperadores) && OperadorUnario(topoPilha(pilhaOperadores))) {
                char *func = desempilha(pilhaOperadores);
                strcat(resultado, func);
                strcat(resultado, " ");
                free(func);
            }
        }

        token = strtok(NULL, " ");
    }

    // Desempilha o restante dos operadores
    while (!pilhaVazia(pilhaOperadores)) {
        char *op = desempilha(pilhaOperadores);
        strcat(resultado, op);
        strcat(resultado, " ");
        free(op);
    }

    liberarPilha(pilhaOperadores);

    // Remove espaço extra ao final
    int len = strlen(resultado);
    if (len > 0 && resultado[len - 1] == ' ') {
        resultado[len - 1] = '\0';
    }

    return resultado;
}

// Avaliação Pós-fixa
float getValorPosFixa(char *strPosFixa) {
    float valores[MAX];
    int topo = -1;
    char strCopia[MAX_StrInFixa];
    strcpy(strCopia, strPosFixa);

    char *pToken = strtok(strCopia, " ");
    while (pToken != NULL) {
        if (Digito(pToken[0]) || (pToken[0] == '-' && Digito(pToken[1])) || pToken[0] == '.') {
            valores[++topo] = atof(pToken);
        } else if (OperadorBinario(pToken)) {
            if (topo < 1) return NAN;
            float b = valores[topo--];
            float a = valores[topo--];
            valores[++topo] = aplicarOperador(a, b, pToken[0]);
        } else if (OperadorUnario(pToken)) {
            if (topo < 0) return NAN;
            float a = valores[topo--];
            valores[++topo] = aplicarFuncaoUnaria(pToken, a);
        }
        pToken = strtok(NULL, " ");
    }

    return (topo == 0) ? valores[topo] : NAN;
}

// Avaliação Infixa (interna chama a Pós-fixa)
float getValorInFixa(char *StrInFixa) {
    char expressaoComEspacos[MAX_StrInFixa * 2] = "";
    for (int i = 0; StrInFixa[i] != '\0'; i++)
    {
        char c = StrInFixa[i];
        if (c == '(' || c == ')' || c == '+' || c == '-' || c == '*' ||
            c == '/' || c == '%' || c == '^')
        {
            strcat(expressaoComEspacos, " ");
            strncat(expressaoComEspacos, &c, 1);
            strcat(expressaoComEspacos, " ");
        }
        else
        {
            strncat(expressaoComEspacos, &c, 1);
        }
    }

    char *posFixa = getFormaPosFixa(expressaoComEspacos);
    if (!posFixa) return NAN;

    float valor = getValorPosFixa(posFixa);
    free(posFixa);
    return valor;
}
