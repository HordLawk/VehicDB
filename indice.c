#include <stdio.h>
#include <stdlib.h>
#include "indice.h"

void escrever_indices(Indice *indices, int qtd, FILE *stream, char tipo){
    for (int i = 0; i < qtd; i++){
        fwrite(&indices[i].id, sizeof(int), 1, stream);
        if (tipo == '1')
            fwrite(&indices[i].RRN, sizeof(int), 1, stream);
        else if (tipo == '2')
            fwrite(&indices[i].byteOffset, sizeof(long), 1, stream);
    }   
}

void ordenar_indices(Indice *indices, int qtd, char tipo){
    for (int i = 0; i < qtd - 1; i++){
        for (int j = i + 1; j < qtd; j++){
            if (indices[i].id > indices[j].id){
                Indice aux = indices[i];
                indices[i] = indices[j];
                indices[j] = indices[i];
            }
        }
    }
}

void mostrar_indices(Indice *indices, int qtd, char tipo){
    if (tipo == '1'){
        for (int i = 0; i < qtd; i++)
            printf("ID: %d, RRN: %d\n", indices[i].id, indices[i].RRN);
    }
    else if (tipo == '2'){
        for (int i = 0; i < qtd; i++)
            printf("ID: %d, Offset: %li\n", indices[i].id, indices[i].byteOffset);
    }
}

Indice *ler_indices(FILE *stream, int *qtd, char tipo){
    // verificacao do arquivo
    char status;
    fread(&status, sizeof(char), 1, stream);
    if (status == '0'){
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    // leitura dos indices
    Indice *indices = NULL;
    int qtd_ind = 0;
    char c;
    while ((c = fgetc(stream)) != EOF){
        ungetc(c, stream);

        Indice *aux = realloc(indices, ((qtd_ind + 1) * sizeof(Indice)));
        indices = aux;                    
        fread(&indices[qtd_ind].id, sizeof(int), 1, stream);
        if (tipo == '1')
            fread(&indices[qtd_ind].RRN, sizeof(int), 1, stream);
        else if (tipo == '2')
            fread(&indices[qtd_ind].byteOffset, sizeof(long), 1, stream);
        qtd_ind++;
    }

    // retorno da funcao
    *qtd = qtd_ind;
    return indices;
}
 
int busca_indices(Indice *indices, int inicio, int fim, int id){
    if (fim >= inicio) {
        int meio = inicio + (fim - inicio) / 2;

        if (indices[meio].id == id)
            return meio;
        if (indices[meio].id > id)
            return busca_indices(indices, inicio, meio - 1, id);
        return busca_indices(indices, meio+1, fim, id);
    }

    return -1;
}
