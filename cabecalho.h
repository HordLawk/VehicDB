#include <stdio.h>

/* struct com os dados de um cabecalho,
topo1 e proxRRn sao utilizados apenas com arquivos tipo1
topo2 e proxByteOffset sao utilizados apenas com arquivos tipo2 */
typedef struct cabecalho
{
    char status, codC5, codC6, codC7;
    int topo1, proxRRN, nroRegRem;
    long topo2, proxByteOffset;
    char descricao[40], desC1[22], desC2[19], desC3[24], desC4[8], desC5[16], desC6[18], desC7[19];

} cabecalho;

/* escreve os dados de um cabecalho em um arquivo binario, dependendo do seu tipo
parametros: cabecalho, ponteiro para arquivo, tipo do arquivo */
void escrever_cabecalho(cabecalho c, FILE *stream, char tipo);

/* le os dados de um cabecalho de um arquivo binario, dependendo do seu tipo; retorna cabecalho
parametros: ponteiro para arquivo, tipo do arquivo */
cabecalho ler_cabecalho(FILE *stream, char tipo);