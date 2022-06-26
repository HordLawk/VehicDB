#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
            if (indices[i].id >= indices[j].id){
                Indice aux = indices[i];
                indices[i] = indices[j];
                indices[j] = aux;
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

void funcionalidade_5(FILE *bin, FILE *ind, char *tipo){
    char status = '0';
    fwrite(&status, sizeof(char), 1, ind);
    
    Indice *indices = NULL; // vetor de indices
    int qtd_ind = 0; // quantidade de indices no vetor

    // se tipo selecionado for "tipo1"
    if (strcmp(tipo, "tipo1") == 0){
        // leitura dos registros e criacao dos indices
        int RRN = -1;
        char removido;
        while (fread(&removido, 1, 1, bin), !feof(bin)){
            RRN++;

            // verificar se registro foi removido
            if (removido == '1'){
                fseek(bin, 96, SEEK_CUR);
                continue;
            }
            fseek(bin, 4, SEEK_CUR);

            // criar indice que representa registro lido
            Indice *aux = realloc(indices, ((qtd_ind + 1) * sizeof(Indice)));
            indices = aux;                    
            indices[qtd_ind].RRN = RRN;
            fread(&indices[qtd_ind].id, sizeof(int), 1, bin);
            qtd_ind++;

            // mover ponteiro para proximo registro
            fseek(bin, 88, SEEK_CUR);
        }
        ordenar_indices(indices, qtd_ind, '1');
        escrever_indices(indices, qtd_ind, ind, '1');
    }
    // se tipo selecionado for "tipo2"
    else if (strcmp(tipo, "tipo2") == 0){
        // leitura dos registros
        char removido;
        while (fread(&removido, 1, 1, bin), !feof(bin)){
            // verificar se registro foi removido
            int tamRegistro;
            fread(&tamRegistro, 4, 1, bin);
            if (removido == '1'){
                fseek(bin, tamRegistro, SEEK_CUR);
                continue;
            }
            fseek(bin, 8, SEEK_CUR);

            // criar indice que representa registro lido
            long proxByteOffset = ftell(bin) - 1 - 8 - 4;
            Indice *aux = realloc(indices, ((qtd_ind + 1) * sizeof(Indice)));
            indices = aux;
            indices[qtd_ind].byteOffset = proxByteOffset;
            fread(&indices[qtd_ind].id, sizeof(int), 1, bin);
            qtd_ind++;

            // mover ponteiro para proximo registro
            fseek(bin, tamRegistro - 4 - 8, SEEK_CUR); 
        }
        ordenar_indices(indices, qtd_ind, '2');
        escrever_indices(indices, qtd_ind, ind, '2');
    }
    free(indices);

    // atualizar status do arquivo
    status = '1';
    fseek(ind, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, ind);
}