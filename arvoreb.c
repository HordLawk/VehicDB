#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "indice.h"
#include "arvoreb.h"

// nao sei se e melhor ler os -1 logo fingindo que e algo valido
// ou ler so os validos e dps preencher o resto com -1
Indice_b ler_indice_b(FILE *stream, char tipo){
    Indice_b ind;
    fread(&ind.tipo, sizeof(char), 1, stream);
    fread(&ind.nro_chaves, sizeof(int), 1, stream);
    if (tipo == '1'){
        for (int i = 0; i < ORDEM-1; i++){
            fread(&ind.chaves[i].id, sizeof(int), 1, stream);
            fread(&ind.chaves[i].RRN, sizeof(int), 1, stream);
        }
    }
    else if (tipo == '2'){
        for (int i = 0; i < ORDEM-1; i++){
            fread(&ind.chaves[i].id, sizeof(int), 1, stream);
            fread(&ind.chaves[i].byteOffset, sizeof(long), 1, stream);
        }
    }
    for (int i = 0; i < ORDEM; i++){
        fread(&ind.desc[i], sizeof(int), 1, stream);
    }

    return ind;
}

// nao sei se e melhor ter os -1 logo fingindo que e algo valido
// ou escrever so os validos e dps preencher o resto com -1
void escrever_indice_b(FILE *stream, Indice_b ind, char tipo){
    fwrite(&ind.tipo, sizeof(char), 1, stream);
    fwrite(&ind.nro_chaves, sizeof(int), 1, stream);
    if (tipo == '1'){
        for (int i = 0; i < ORDEM-1; i++){
            fwrite(&ind.chaves[i].id, sizeof(int), 1, stream);
            fwrite(&ind.chaves[i].RRN, sizeof(int), 1, stream);
        }
    }
    else if (tipo == '2'){
        for (int i = 0; i < ORDEM-1; i++){
            fwrite(&ind.chaves[i].id, sizeof(int), 1, stream);
            fwrite(&ind.chaves[i].byteOffset, sizeof(long), 1, stream);
        }
    }
    for (int i = 0; i < ORDEM; i++){
        fwrite(&ind.desc[i], sizeof(int), 1, stream);
    }
}

// nao gostei das variaiveis serem i, id, ind todos nomes iguais
int buscar_indice_b1(FILE *stream, Indice_b ind, int id){
    int i = 0;
    while (i < ind.nro_chaves && id > ind.chaves[i].id)
        i++;

    // encontrou o id procurado
    if (i < ind.nro_chaves && id == ind.chaves[i].id){
        return ind.chaves[i].RRN;
    }
    // nao encontrou o id e nao tem mais descendentes
    else if (ind.tipo == '2'){
        return -1;
    }
    // procurar id no descendente
    else{
        fseek(stream, ((ind.desc[i]+1) * TAM_ARVB1), SEEK_SET);
        Indice_b ind2 = ler_indice_b(stream, '1');
        return buscar_indice_b1(stream, ind2, id);
    }
}

long buscar_indice_b2(FILE *stream, Indice_b ind, int id){
    int i = 0;
    while (i < ind.nro_chaves && id > ind.chaves[i].id)
        i++;

    // encontrou o id procurado
    if (i < ind.nro_chaves && id == ind.chaves[i].id){
        return ind.chaves[i].byteOffset;
    }
    // nao encontrou o id e nao tem mais descendentes
    else if (ind.tipo == '2'){
        return -1;
    }
    // procurar id no descendente
    else{
        fseek(stream, ((ind.desc[i]+1) * TAM_ARVB2), SEEK_SET);
        Indice_b ind2 = ler_indice_b(stream, '2');
        return buscar_indice_b2(stream, ind2, id);
    }
}


//

