typedef struct Indice{
    int id, RRN;
    long byteOffset;
} Indice;

void escrever_indices(Indice *indices, int qtd, FILE *stream, char tipo);
void ordenar_indices(Indice *indices, int qtd, char tipo);
void desalocar_indices(Indice *indices, int qtd);
void mostrar_indices(Indice *indices, int qtd, char tipo);
