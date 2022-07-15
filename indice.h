/* struct com os dados de um indice, armazena
o id de um veiculo e seu RRN/byteOffset */ 
typedef struct Indice{
    int id, RRN;
    long long byteOffset;
} Indice;

/* escreve os dados dos indices em um arquivo binario
parametros: vetor de indices, quantidade de indices, ponteiro para arquivo, 
            tipo do arquivo de veiculos indexado */          
void escrever_indices(Indice *indices, int qtd, FILE *stream, char tipo);

/* le os dados de um arquivo binario de indices; retorna vetor com os indices
parametros: ponteiro para arquivo, tipo do arquivo de veiculos indexado,
            quantidade de indices lidos (passado por referência) */
Indice *ler_indices(FILE *stream, int *qtd, char tipo);

/* busca binaria para encontrar um id no vetor de indices; retorna posicao do id no vetor ou -1
parametros: vetor de indices, posicao inicial do vetor, posicao final do vetor, id desejado */
int busca_indices(Indice *indices, int inicio, int fim, int id);

/* funcao que compara o id de dois indices implementada para uso da qsort() na ordenacao;
retorna 1 se a.id > b.id, -1 se a.id < b.id, ou 0 se a.id == b.id
parametros: ponteiro para indice a, ponteiro para indice b */
int comparar_indices (const void *a, const void *b);

/* executa a funcionalidade 5: criar um arquivo de indices baseado em um arquivo de veiculos
parametros: ponteiro para arquivo de veiculos, ponteiro para arquivo de indices,
            tipo do arquivo de veiculos  */ 
void criar_arquivo_indices(FILE *bin, FILE *ind, char *tipo);