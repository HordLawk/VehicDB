#include <stdio.h>

typedef struct cabecalho
{
    char status, codC5, codC6, codC7;
    int topo1, proxRRN, nroRegRem;
    long topo2, proxByteOffset;
    char descricao[40], desC1[22], desC2[19], desC3[24], desC4[8], desC5[16], desC6[18], desC7[19];

} cabecalho;

// escrever cabecalho
void escrever_cabecalho(cabecalho c, FILE *stream, char tipo);
cabecalho ler_cabecalho(FILE *stream, char tipo);