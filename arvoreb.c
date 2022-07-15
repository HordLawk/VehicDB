#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvoreb.h"

cabecalho_arvb ler_cabecalho_arvb(FILE *stream){
    cabecalho_arvb cab;
    fread(&cab.status, sizeof(char), 1, stream);
    fread(&cab.noRaiz, sizeof(int), 1, stream);
    fread(&cab.proxRRN, sizeof(int), 1, stream);
    fread(&cab.nroNos, sizeof(int), 1, stream);
    return cab;
}

void escrever_cabecalho_arvb(cabecalho_arvb cab, FILE *stream, char tipo){
    fwrite(&cab.status, sizeof(char), 1, stream);
    fwrite(&cab.noRaiz, sizeof(int), 1, stream);
    fwrite(&cab.proxRRN, sizeof(int), 1, stream);
    fwrite(&cab.nroNos, sizeof(int), 1, stream);
    int tamLixo = (tipo == '1' ? TAM_ARVB1 : TAM_ARVB2) - 13;
    char lixo = '$';
    while(tamLixo--) fwrite(&lixo, sizeof(char), 1, stream);
}

no_arvb ler_no_arvb(FILE *stream, char tipo){
    no_arvb no;
    fread(&no.tipo, sizeof(char), 1, stream);
    fread(&no.nro_chaves, sizeof(int), 1, stream);
    if (tipo == '1'){
        for (int i = 0; i < ORDEM-1; i++){
            fread(&no.chaves[i].id, sizeof(int), 1, stream);
            fread(&no.chaves[i].RRN, sizeof(int), 1, stream);
        }
    }
    else if (tipo == '2'){
        for (int i = 0; i < ORDEM-1; i++){
            fread(&no.chaves[i].id, sizeof(int), 1, stream);
            fread(&no.chaves[i].byteOffset, sizeof(long), 1, stream);
        }
    }
    for (int i = 0; i < ORDEM; i++)
        fread(&no.desc[i], sizeof(int), 1, stream);

    return no;
}

void escrever_no_arvb(FILE *stream, no_arvb no, char tipo){
    fwrite(&no.tipo, sizeof(char), 1, stream);
    fwrite(&no.nro_chaves, sizeof(int), 1, stream);
    if (tipo == '1'){
        for (int i = 0; i < ORDEM-1; i++){
            fwrite(&no.chaves[i].id, sizeof(int), 1, stream);
            fwrite(&no.chaves[i].RRN, sizeof(int), 1, stream);
        }
    }
    else if (tipo == '2'){
        for (int i = 0; i < ORDEM-1; i++){
            fwrite(&no.chaves[i].id, sizeof(int), 1, stream);
            fwrite(&no.chaves[i].byteOffset, sizeof(long), 1, stream);
        }
    }
    for (int i = 0; i < ORDEM; i++)
        fwrite(&no.desc[i], sizeof(int), 1, stream);
}

int buscar_arvb1(FILE *stream, no_arvb atual, int id){
    int i = 0;
    while (i < atual.nro_chaves && id > atual.chaves[i].id)
        i++;
    // encontrou o id procurado
    if (i < atual.nro_chaves && id == atual.chaves[i].id){
        return atual.chaves[i].RRN;
    }
    // nao encontrou o id e nao tem mais descendentes
    else if (atual.tipo == '2'){
        return -1;
    }
    // procurar id no descendente
    else{
        fseek(stream, ((atual.desc[i]+1) * TAM_ARVB1), SEEK_SET);
        no_arvb desc = ler_no_arvb(stream, '1');
        return buscar_arvb1(stream, desc, id);
    }
}

long buscar_arvb2(FILE *stream, no_arvb atual, int id){
    int i = 0;
    while (i < atual.nro_chaves && id > atual.chaves[i].id)
        i++;
    // encontrou o id procurado
    if (i < atual.nro_chaves && id == atual.chaves[i].id){
        return atual.chaves[i].byteOffset;
    }
    // nao encontrou o id e nao tem mais descendentes
    else if (atual.tipo == '2'){
        return -1;
    }
    // procurar id no descendente
    else{
        fseek(stream, ((atual.desc[i]+1) * TAM_ARVB2), SEEK_SET);
        no_arvb desc = ler_no_arvb(stream, '2');
        return buscar_arvb2(stream, desc, id);
    }
}

int insercao(FILE *stream, cabecalho_arvb *cab, int RRN_atual, Indice chave, Indice *promo_chave, int *promo_desc_dir, char tipo){
    // chegou no fim da arvore, voltar recursivamente
    if (RRN_atual == -1){
        *promo_chave = chave;
        *promo_desc_dir = -1;
        return 1;
    }
    // no existente da arvore
    else{
        // leitura do no atual
        int tamNo = tipo == '1' ? TAM_ARVB1 : TAM_ARVB2;
        fseek(stream, (RRN_atual + 1)*tamNo, SEEK_SET);
        no_arvb atual = ler_no_arvb(stream, tipo);

        // encontrar posicao adequada da chave no nó atual
        int pos = 0;
        while (pos < atual.nro_chaves - 1 && chave.id > atual.chaves[pos].id)
            pos++;
        if (pos == 0 && chave.id < atual.chaves[pos].id) pos = -1;
        
        // inserir a chave no filho adequado
        int retorno_RRN;
        Indice retorno_chave;
        int retorno = insercao(stream, cab, atual.desc[pos+1], chave, &retorno_chave, &retorno_RRN, tipo);
        
        // nao houve promocao -> apenas retornar a recursao
        if (retorno == 0)
            return retorno;
        // houve promocao -> inserir chave promovida no atual
        else{ 
            // tem espaco -> inserir no nó atual na posicao adequada
            if (atual.nro_chaves < ORDEM-1){
                int pos = atual.nro_chaves - 1;
                while (pos >= 0 && retorno_chave.id < atual.chaves[pos].id){
                    atual.chaves[pos+1] = atual.chaves[pos];
                    atual.desc[pos+2] = atual.desc[pos+1];
                    pos--;
                }
                atual.chaves[pos+1] = retorno_chave;
                atual.desc[pos+2] = retorno_RRN;
                atual.nro_chaves++;

                fseek(stream, (RRN_atual + 1)*tamNo, SEEK_SET);
                escrever_no_arvb(stream, atual, tipo);
                return 0;
            }
            // nao tem espaco -> realizar o procedimento split();
            else{
                no_arvb novo = novo_no();
                split(cab, &atual, &novo, retorno_chave, retorno_RRN, promo_chave, promo_desc_dir);
            
                // no esquerdo
                fseek(stream, (RRN_atual + 1)*tamNo, SEEK_SET);
                escrever_no_arvb(stream, atual, tipo);
                // no direito
                fseek(stream, (*promo_desc_dir + 1)*tamNo, SEEK_SET);
                escrever_no_arvb(stream, novo, tipo);

                return 1;
            }
        }
    }
}

void split(cabecalho_arvb *cab, no_arvb *atual, no_arvb *novo, Indice ins_chave, int ins_RRN, Indice *promo_chave, int *promo_desc_dir){
    Indice chaves[ORDEM];
    int desc[ORDEM+1];

    // copiar as chaves e descendentes da pagina atual para um vetor com +1 espaco
    for (int i = 0; i < ORDEM-1; i++){
        chaves[i] = atual->chaves[i];
        desc[i] = atual->desc[i];
    }
    desc[ORDEM-1] = atual->desc[ORDEM-1];
    desc[ORDEM] = -1;

    // inserir a chave e descendente na posicao adequada dos vetores
    int i = ORDEM - 2;
    while (i >= 0 && ins_chave.id < chaves[i].id){
        chaves[i+1] = chaves[i];
        desc[i+2] = desc[i+1];
        i--;
    }
    chaves[i+1] = ins_chave;
    desc[i+2] = ins_RRN;

    // indicar a chave e descendente que vao ser promovidos
    *promo_chave = chaves[ORDEM/2];
    *promo_desc_dir = cab->proxRRN;
    cab->proxRRN++;
    cab->nroNos++;

    char tipoNo = atual->tipo;
    *atual = novo_no(); // resetar os valores do no

    // copiar os valores a esquerda da chave promovida para o no esquerdo 
    for (int i = 0; i < ORDEM/2; i++){
        atual->chaves[i] = chaves[i];
        atual->desc[i] = desc[i];
        atual->nro_chaves++;
    }
    atual->desc[atual->nro_chaves] = desc[(ORDEM/2)];

    // copiar os valores a direita da chave promovida para o no direito
    for (int i = ORDEM/2 + 1; i < ORDEM; i++){
        novo->chaves[novo->nro_chaves] = chaves[i];
        novo->desc[novo->nro_chaves] = desc[i];
        novo->nro_chaves++;
    }
    novo->desc[novo->nro_chaves] = desc[ORDEM];

    // atualizar o tipo do no
    if (tipoNo == '0'){
        if (desc[0] == -1) atual->tipo = novo->tipo = '2';
        else atual->tipo = novo->tipo = '1';
    }
    else{
        atual->tipo = novo->tipo = tipoNo;
    }
}

no_arvb novo_no(){
    no_arvb novo;
    novo.tipo = '2';
    novo.nro_chaves = 0;
    for (int i = 0; i < ORDEM-1; i++){
        novo.desc[i] = -1;
        novo.chaves[i].id = -1;
        novo.chaves[i].RRN = -1;
        novo.chaves[i].byteOffset = -1;
    }
    novo.desc[ORDEM-1] = -1;
    return novo;
}

void inserir_arvb(FILE *stream, cabecalho_arvb *cab, Indice chave, char tipo){
    int tamNo = tipo == '1' ? TAM_ARVB1 : TAM_ARVB2;

    // nao tem raiz -> criar nova raiz e inserir nova chave
    if (cab->noRaiz == -1){
        no_arvb raiz = novo_no();
        raiz.tipo = '0';
        raiz.chaves[0] = chave;
        raiz.nro_chaves++;
        fseek(stream, (cab->proxRRN + 1)*tamNo, SEEK_SET);
        escrever_no_arvb(stream, raiz, tipo);

        cab->noRaiz = cab->proxRRN;
        cab->proxRRN++;
        cab->nroNos++;
    }
    // tem raiz -> inserir chave usando metodo insercao
    else{
        Indice promo_chave;   // chave promovida
        int promo_desc_dir;   // descendente direito promovido

        int retorno = insercao(stream, cab, cab->noRaiz, chave, &promo_chave, &promo_desc_dir, tipo);
        // existiu promocao apos a insercao na raiz -> criar nova raiz com valores promovidos
        if (retorno == 1){
            no_arvb novaRaiz = novo_no();
            novaRaiz.chaves[0] = promo_chave;
            novaRaiz.desc[0] = cab->noRaiz;
            novaRaiz.desc[1] = promo_desc_dir;
            novaRaiz.nro_chaves++;
            novaRaiz.tipo = '0';
            fseek(stream, (cab->proxRRN + 1)*tamNo, SEEK_SET);
            escrever_no_arvb(stream, novaRaiz, tipo);

            cab->noRaiz = cab->proxRRN;
            cab->proxRRN++;
            cab->nroNos++;
        }
    }
}