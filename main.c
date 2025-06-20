#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expressao.h"

int main() {

    Expressao expressoes[] = {// escrita por espaço de acordo com a tabela
        {"3 4 + 5 *", "( 3 + 4 ) * 5", 35.0},
        {"7 2 * 4 +", "7 * 2 + 4", 18.0},
        {"8 5 2 4 + * +", "8 + ( 5 * ( 2 + 4 ) )", 38.0},
        {"6 2 / 3 + 4 *", "( 6 / 2 + 3 ) * 4", 24.0},
        {"9 5 2 8 * 4 + * +", "9 + ( 5 * ( 2 * 8 + 4 ) )", 109.0}, 
        {"2 3 + log 5 /", "log ( 2 + 3 ) / 5", 0.14},
        {"10 log 3 ^ 2 +", "( log ( 10 ) ^ 3 ) + 2", 3.0}, 
        {"45 60 + 30 cos *", "( 45 + 60 ) * cos ( 30 )", 90.93},
        {"45 sen 2 ^ 0.5 +", "sen ( 45 ) ^ 2 + 0.5", 1.0},
        {"1 1 +", " 1 + 1", 2.0},

    };


    int n = sizeof(expressoes) / sizeof(Expressao);


    for (int i = 0; i < n; i++) {
        printf("---------------------------------------\n");
        printf("Teste %d:\nResultado esperado: %.2f\n", i + 1, expressoes[i].Valor);
        printf("\n");
        // Conversão pós-fixa → infixa
        printf("Valor de Posfixa: %s\n", expressoes[i].posFixa);
        char *posFixa_ParaInfixa = getFormaInFixa(expressoes[i].posFixa);
        printf("De Posfixa para Infixa: %s\n", posFixa_ParaInfixa);
        float valorInf = getValorInFixa(posFixa_ParaInfixa);
        printf("> Calculo de Posfixa convertida: %.2f\n", valorInf);
        // Libera memória alocada
        free(posFixa_ParaInfixa);
        printf("\n");
        //Conversão infixa -> pós-fixa
        printf("Valor infixa: %s\n", expressoes[i].inFixa);
        char *inFixa_ParaPosFixa = getFormaPosFixa(expressoes[i].inFixa);
        printf("De Infixa para posfixa: %s\n", inFixa_ParaPosFixa);
        float valorPos= getValorPosFixa(inFixa_ParaPosFixa);
        printf("> Calculo de Infixa convertida: %.2f\n", valorPos);

        free(inFixa_ParaPosFixa);
        
    }

    return 0;
}
