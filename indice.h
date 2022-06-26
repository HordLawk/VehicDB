typedef struct Indice{
    int id, RRN;
    long byteOffset;
} Indice;

void escrever_indices(Indice *indices, int qtd, FILE *stream, char tipo);
void ordenar_indices(Indice *indices, int qtd, char tipo);
void desalocar_indices(Indice *indices, int qtd);
void mostrar_indices(Indice *indices, int qtd, char tipo);
Indice *ler_indices(FILE *stream, int *qtd, char tipo);
//int busca_indices(Indice *indices, int inicio, int fim, int id);
void funcionalidade_5(FILE *bin, FILE *ind, char *tipo);