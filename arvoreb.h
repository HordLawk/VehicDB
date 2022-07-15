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


Indice_b criar_no_teste();
void split_teste(Cabecalho_b *cab, Indice i_key, int i_RRN, Indice_b *atual, Indice *promo_key, int *promo_r_child, Indice_b *newpage, char tipo);
int insercao_teste(FILE *stream, Cabecalho_b *cab, int RRN_atual, Indice chave, int *RRN_promo_r, Indice *chave_promo, char tipo);
void driver(FILE *stream, Cabecalho_b *c, Indice key, char tipo);
void mostrar_no(Indice_b atual, char tipo);
#endif