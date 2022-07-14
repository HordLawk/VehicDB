#ifndef _ARVOREB_H
#define _ARVOREB_H
#include "indice.h"
#define ORDEM 4
#define TAM_ARVB1 45
#define TAM_ARVB2 57

/* struct com os dados de indice da arvore b
*/
typedef struct Indice_b{
    char tipo;
    int nro_chaves;
    Indice chaves[ORDEM];
    int desc[ORDEM + 1]; 
} Indice_b;

typedef struct Cabecalho_b{
    char status;
    int noRaiz, proxRRN, nroNos;
} Cabecalho_b;

Indice_b ler_indice_b(FILE *stream, char tipo);
void escrever_indice_b(FILE *stream, Indice_b ind, char tipo);
int buscar_indice_b1(FILE *stream, Indice_b ind, int id);
long buscar_indice_b2(FILE *stream, Indice_b ind, int id);
void inserir_indice_b(FILE *stream, Indice ind, char tipo);
Indice_b *alg_insercao_b(FILE *stream, Indice_b *no, Indice ind, char tipo, Cabecalho_b *cabecalho);
void escrever_cabecalho_b(Cabecalho_b c, FILE *stream, char tipo);
Cabecalho_b ler_cabecalho_b(FILE *stream, char tipo);
#endif