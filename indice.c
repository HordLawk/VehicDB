#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "indice.h"
#include "utils.h"

void escrever_indices(Indice *indices, int qtd, FILE *stream, char tipo){
    if (tipo == '1'){
        for (int i = 0; i < qtd; i++){
            fwrite(&indices[i].id, sizeof(int), 1, stream);
            fwrite(&indices[i].RRN, sizeof(int), 1, stream);
        }
    }
    else if (tipo == '2'){
        for (int i = 0; i < qtd; i++){
            fwrite(&indices[i].id, sizeof(int), 1, stream);
            fwrite(&indices[i].byteOffset, sizeof(long), 1, stream);
        }
    }
}

Indice *ler_indices(FILE *stream, int *qtd, char tipo){
    // leitura dos indices
    Indice *indices = NULL;
    int qtd_ind = 0;
    unsigned char c = fgetc(stream);
    while (!feof(stream)){
        ungetc(c, stream);

        Indice *aux = realloc(indices, ((qtd_ind + 1) * sizeof(Indice)));
        indices = aux;         
        fread(&indices[qtd_ind].id, sizeof(int), 1, stream);
        if (tipo == '1') 
            fread(&indices[qtd_ind].RRN, sizeof(int), 1, stream);
        else if (tipo == '2')
            fread(&indices[qtd_ind].byteOffset, sizeof(long), 1, stream);
        qtd_ind++;
        c = fgetc(stream);
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

int comparar_indices (const void *a, const void *b) {
    if (((Indice *)a)->id > ((Indice *)b)->id)
        return 1;
    if (((Indice *)a)->id < ((Indice *)b)->id)
        return -1;
    return 0;
}

void criar_arquivo_indices(FILE *bin, FILE *ind, char *tipo){
    char status = '0';
    fwrite(&status, sizeof(char), 1, ind);
    
    Indice *indices = NULL; // vetor de indices
    int qtd_ind = 0; // quantidade de indices no vetor

    // se tipo selecionado for "tipo1"
    if (strcmp(tipo, "tipo1") == 0){
        // leitura dos registros
        int RRN = -1;
        char removido;
        while (fread(&removido, sizeof(char), 1, bin), !feof(bin)){
            RRN++;
            // verificar se registro foi removido
            if (removido == '1'){
                fseek(bin, TAM_TIPO1 - sizeof(char), SEEK_CUR);
                continue;
            }
            fseek(bin, sizeof(int), SEEK_CUR);

            // criar indice que representa registro lido
            Indice *aux = realloc(indices, ((qtd_ind + 1) * sizeof(Indice)));
            indices = aux;                    
            indices[qtd_ind].RRN = RRN;
            fread(&indices[qtd_ind].id, sizeof(int), 1, bin);
            qtd_ind++;

            // mover ponteiro para proximo registro
            fseek(bin, TAM_TIPO1 - sizeof(char) - (2 * sizeof(int)), SEEK_CUR);
        }
    }
    // se tipo selecionado for "tipo2"
    else if (strcmp(tipo, "tipo2") == 0){
        // leitura dos registros
        char removido;
        while (fread(&removido, sizeof(char), 1, bin), !feof(bin)){
            // verificar se registro foi removido
            int tamRegistro;
            fread(&tamRegistro, sizeof(int), 1, bin);
            if (removido == '1'){
                fseek(bin, tamRegistro, SEEK_CUR);
                continue;
            }
            fseek(bin, sizeof(long), SEEK_CUR);

            // criar indice que representa registro lido
            long proxByteOffset = ftell(bin) - sizeof(char) - sizeof(int) - sizeof(long);
            Indice *aux = realloc(indices, ((qtd_ind + 1) * sizeof(Indice)));
            indices = aux;
            indices[qtd_ind].byteOffset = proxByteOffset;
            fread(&indices[qtd_ind].id, sizeof(int), 1, bin);
            qtd_ind++;

            // mover ponteiro para proximo registro
            fseek(bin, tamRegistro - sizeof(int) - sizeof(long), SEEK_CUR); 
        }
    }
    // ordenacao e escrita no arquivo
    qsort(indices, qtd_ind, sizeof(Indice), comparar_indices);
    escrever_indices(indices, qtd_ind, ind, tipo[4]);
    free(indices);

    // atualizar status do arquivo
    status = '1';
    fseek(ind, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, ind);
}