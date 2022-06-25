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
