#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void inserir_indice_b(FILE *stream, Indice ind, char tipo){
    Cabecalho_b cabecalho = ler_cabecalho_b(stream, tipo);
    char tamNo = tipo == '1' ? TAM_ARVB1 : TAM_ARVB2;
    if(cabecalho.noRaiz == -1){
        Indice_b ind_b;
        ind_b.tipo = '0';
        ind_b.nro_chaves = 1;
        ind_b.chaves[0] = ind;
        for(int i = 1; i < ORDEM; i++) ind_b.chaves[i].byteOffset = ind_b.chaves[i].RRN = ind_b.chaves[i].id = -1;
        for(int i = 0; i <= ORDEM; i++) int_b.desc[i] = -1;
        fseek(stream, tamNo, SEEK_SET);
        escrever_indice_b(stream, ind_b, tipo);
        cabecalho.noRaiz = 0;
        cabecalho.proxRRN = 1;
        cabecalho.nroNos = 1;
        fseek(stream, 0, SEEK_SET);
        escrever_cabecalho_b(cabecalho, stream, tipo);
        return;
    }
    fseek(stream, ((cabecalho.noRaiz + 1) * tamNo), SEEK_SET);
    Indice_b raiz = ler_indice_b(stream, tipo);
    Indice_b *novoNo = alg_insercao_b(stream, &raiz, ind, tipo, &cabecalho);
    if(!novoNo) return;
    raiz.tipo = '1';
    fseek(stream, ((cabecalho.noRaiz + 1) * tamNo), SEEK_SET);
    escrever_indice_b(stream, raiz, tipo);
    fseek(stream, (cabecalho.proxRRN + 1) * tamNo, SEEK_SET);
    escrever_indice_b(stream, *novoNo, tipo);
    cabecalho.nroNos++;
    Indice_b novaRaiz = {
        '0',
        1,
    };
    novaRaiz.chaves[0] = raiz.chaves[raiz.nro_chaves];
    raiz.chaves[raiz.nro_chaves].byteOffset = raiz.chaves[raiz.nro_chaves].RRN = raiz.chaves[raiz.nro_chaves].id = -1;
    for(int i = 1; i < ORDEM; i++) novaRaiz.chaves[i].byteOffset = novaRaiz.chaves[i].RRN = novaRaiz.chaves[i].id = -1;
    novaRaiz.desc[0] = cabecalho.noRaiz;
    novaRaiz.desc[1] = cabecalho.proxRRN++;
    cabecalho.noRaiz = cabecalho.proxRRN;
    for(int i = 2; i <= ORDEM; i++) novaRaiz.desc[i] = -1;
    fseek(stream, tamNo, SEEK_CUR);
    escrever_indice_b(stream, novaRaiz, tipo);
    cabecalho.nroNos++;
    fseek(stream, 0, SEEK_SET);
    escrever_cabecalho_b(cabecalho, stream, tipo);
}

Indice_b *alg_insercao_b(FILE *stream, Indice_b *no, Indice ind, char tipo, Cabecalho_b *cabecalho){
    char tamNo = tipo == '1' ? TAM_ARVB1 : TAM_ARVB2;
    int i = 0;
    while (i < no->nro_chaves && ind.id > no->chaves[i].id) i++;
    if (no->tipo == '2'){
        for(int j = (ORDEM - 1); j > i; j--) no->chaves[j] = no->chaves[j - 1];
        no->chaves[i] = ind;
        if(++no->nro_chaves == ORDEM){
            Indice_b no2 = {
                '2',
                ((ORDEM + 1) >> 1) - 1,
            };
            for(int j = 0; j < ORDEM; j++) no2.chaves[j].byteOffset = no2.chaves[j].RRN = no2.chaves[j].id = -1;
            for(int j = 0; j < no2.nro_chaves; j++) no2.chaves[j] = no->chaves[((ORDEM >> 1) + 1) + j];
            for(int j = 0; j <= ORDEM; j++) no2.desc[j] = -1;
            no->nro_chaves >>= 1;
            for(int j = (no->nro_chaves + 1); j < ORDEM; j++){
                no->chaves[j].byteOffset = no->chaves[j].RRN = no->chaves[j].id = -1;
            }
            return &no2;
        }
        fseek(stream, -tamNo, SEEK_CUR);
        escrever_indice_b(stream, *no, tipo);
        return NULL;
    }
    long no_pos = ftell(stream);
    fseek(stream, ((no->desc[i] + 1) * tamNo), SEEK_SET);
    long desc_pos = ftell(stream);
    Indice_b desc = ler_indice_b(stream, tipo);
    Indice_b *novoNo = alg_insercao_b(stream, &desc, ind, tipo, cabecalho);
    if(!novoNo) return NULL;
    fseek(stream, (cabecalho->proxRRN + 1) * tamNo, SEEK_SET);
    escrever_indice_b(stream, *novoNo, tipo);
    cabecalho->nroNos++;
    for(int j = (ORDEM - 1); j > i; j--) no->chaves[j] = no->chaves[j - 1];
    no->chaves[i] = desc.chaves[desc.nro_chaves];
    desc.chaves[desc.nro_chaves].byteOffset = desc.chaves[desc.nro_chaves].RRN = desc.chaves[desc.nro_chaves].id = -1;
    fseek(stream, desc_pos, SEEK_SET);
    escrever_indice_b(stream, desc, tipo); 
    for(int j = ORDEM; j > (i + 1); j--) no->desc[j] = no->desc[j - 1];
    no->desc[i + 1] = cabecalho->proxRRN++;
    if(++no->nro_chaves == ORDEM){
        Indice_b no2 = {
            '1',
            ((ORDEM + 1) >> 1) - 1,
        };
        for(int j = 0; j < (ORDEM - 1); j++) no2.chaves[j].byteOffset = no2.chaves[j].RRN = no2.chaves[j].id = -1;
        for(int j = 0; j < no2.nro_chaves; j++) no2.chaves[j] = no->chaves[((ORDEM >> 1) + 1) + j];
        for(int j = 0; j < ORDEM; j++) no2.desc[j] = -1;
        for(int j = 0; j <= no2.nro_chaves; j++) no2.desc[j] = no->desc[((ORDEM >> 1) + 1) + j];
        no->nro_chaves >>= 1;
        for(int j = (no->nro_chaves + 1); j < ORDEM; j++){
            no->chaves[j].byteOffset = no->chaves[j].RRN = no->chaves[j].id = -1;
        }
        for(int j = (no->nro_chaves + 1); j <= ORDEM; j++) no->desc[j] = -1;
        return &no2;
    }
    fseek(stream, no_pos, SEEK_SET);
    escrever_indice_b(stream, *no, tipo);
    return NULL;
}

void escrever_cabecalho_b(Cabecalho_b c, FILE *stream, char tipo){
    fwrite(&c.status, sizeof(char), 1, stream);
    fwrite(&c.noRaiz, sizeof(int), 1, stream);
    fwrite(&c.proxRRN, sizeof(int), 1, stream);
    fwrite(&c.nroNos, sizeof(int), 1, stream);
    tamLixo = (tipo == '1' ? TAM_ARVB1 : TAM_ARVB2) - 13;
    char lixo[tamLixo] = {'$'};
    fwrite(lixo, sizeof(char), tamLixo, stream);
}

Cabecalho_b ler_cabecalho_b(FILE *stream, char tipo){
    Cabecalho_b c;
    fread(&c.status, sizeof(char), 1, stream);
    fread(&c.noRaiz, sizeof(int), 1, stream);
    fread(&c.proxRRN, sizeof(int), 1, stream);
    fread(&c.nroNos, sizeof(int), 1, stream);
    return c;
}