#ifndef _ARVOREB_H
#define _ARVOREB_H
#include "indice.h"
#define ORDEM 4
#define TAM_ARVB1 45
#define TAM_ARVB2 57

/* struct com os dados de um cabecalho de arvore b; armazena o status do arquivo,
o RRN do nó raiz, o prox RRN disponivel e o numero de nós da arvore */
typedef struct cabecalho_arvb{
    char status;
    int noRaiz, proxRRN, nroNos;
} cabecalho_arvb;

/* struct com os dados de um nó de arvore b; armazena o tipo do nó,
o numero de chaves, suas chaves e seus descendentes */
typedef struct no_arvb{
    char tipo;
    int nro_chaves;
    Indice chaves[ORDEM];
    int desc[ORDEM + 1]; 
} no_arvb;

/* le os dados de um cabecalho de arquivo de arvore b; retorna cabecalho lido
parametros: ponteiro para arquivo */
cabecalho_arvb ler_cabecalho_arvb(FILE *stream);

/* escreve os dados de um cabecalho de arquivo de arvore b em um arquivo,
parametros: cabecalho a ser escrito, ponteiro para arquivo,
            tipo do arquivo de veiculos indexado */
void escrever_cabecalho_arvb(cabecalho_arvb cab, FILE *stream, char tipo);

/* le os dados de um nó de arvore b de um arquivo; retorna nó lido
parametros: ponteiro para arquivo, tipo do arquivo de veiculos indexado pela arvore b */
no_arvb ler_no_arvb(FILE *stream, char tipo);

/* escreve os dados de um nó de arvore b em um arquivo binario
parametros: ponteiro para arquivo, nó a ser escrito,
            tipo do arquivo de veiculos indexado */
void escrever_no_arvb(FILE *stream, no_arvb no, char tipo);

/* realiza recursivamente a busca de um id em um arquivo de indices arvore b,
usado para arquivos de indice que indexam arquivos de veiculo tipo 1;
retorna RRN relativo ao id procurado, ou -1 se nao encontrar;
parametros: ponteiro para arquivo de indices, nó da arvore b em que deve ser buscado,
            id desejado */
int buscar_arvb1(FILE *stream, no_arvb atual, int id);

/* realiza recursivamente a busca de um id em um arquivo de indices arvore b,
usado para arquivos de indice que indexam arquivos de veiculo tipo 2;
retorna byteOffset relativo ao id procurado, ou -1 se nao encontrar;
parametros: ponteiro para arquivo de indices, nó da arvore b em que deve ser buscado,
            id desejado */
long buscar_arvb2(FILE *stream, no_arvb atual, int id);

/* cria um novo no de arvore b, com todos os valores, chaves e filhos nulos;
retorna no criado */
no_arvb novo_no();

/* insere uma chave em uma arvore b, pode criar a raiz para inserir se a arvore nao existir, 
ou inserir na arvore existente com a funcao insercao(), atualizando a raiz quando necessario;
parametros: ponteiro para arquivo de indices, cabecalho do arquivo de indices (passado por referencia),
            chave a ser inserida na arvore, tipo do arquivo de veiculos indexado. */
void inserir_arvb(FILE *stream, cabecalho_arvb *cab, Indice chave, char tipo);


/* funcao chamada pela funcao inserir_arvb() que insere uma chave na arvore de modo recursivo,
realizando split e inserindo os valores promovidos quando necessario;
retorna 1 se existir promocao, 0 se nao existir promocao;
parametros: ponteiro para arquivo de indices, cabecalho do arquivo de indices (passado por referencia),
            RRN do nó atual, chave a ser inserida na arvore,
            chave promovida e descendente direito promovido (ambos passados por referencia),
            tipo do arquivo de veiculos indexado; */
int insercao(FILE *stream, cabecalho_arvb *cab, int RRN_atual, Indice chave, Indice *promo_chave, int *promo_desc_dir, char tipo);

/* funcao chamada pela funcao insercao(), realiza o split de um nó da arvore (+ nova chave inserida),
define uma chave para ser promovida, insere as chaves à esquerda no nó atual e à direita no novo nó,
atualiza o tipo dos nós criados;
parametros: cabecalho do arquivo de indices (passado por referencia), 
            ponteiro para nó atual e ponteiro para novo nó criado,
            chave e RRN que devem ser inseridos no nó atual,
            chave promovida e descendente direito promovido (ambos passados por referencia) */
void split(cabecalho_arvb *cab, no_arvb *atual, no_arvb *novo, Indice ins_chave, int ins_RRN, Indice *promo_chave, int *promo_desc_dir);


#endif