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

/*
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
*/

void escrever_cabecalho_b(Cabecalho_b c, FILE *stream, char tipo){
    //printf("%c %d %d %d\n", c.status, c.noRaiz, c.proxRRN, c.nroNos);
    fwrite(&c.status, sizeof(char), 1, stream);
    fwrite(&c.noRaiz, sizeof(int), 1, stream);
    fwrite(&c.proxRRN, sizeof(int), 1, stream);
    fwrite(&c.nroNos, sizeof(int), 1, stream);
    int tamLixo = (tipo == '1' ? TAM_ARVB1 : TAM_ARVB2) - 13;
    char lixo = '$';
    for (int i = 0; i < tamLixo; i++)
        fwrite(&lixo, sizeof(char), 1, stream);
}

Cabecalho_b ler_cabecalho_b(FILE *stream, char tipo){
    Cabecalho_b c;
    fread(&c.status, sizeof(char), 1, stream);
    fread(&c.noRaiz, sizeof(int), 1, stream);
    fread(&c.proxRRN, sizeof(int), 1, stream);
    fread(&c.nroNos, sizeof(int), 1, stream);
    return c;
}

void mostrar_no(Indice_b atual, char tipo){
    if (tipo == '1'){
        printf("NO ATUAL:\ntipo: '%c' | nro_chaves: %d\n(%d,%d) (%d,%d) (%d,%d)\n[%d] [%d] [%d] [%d]\n\n", 
        atual.tipo, atual.nro_chaves, 
        atual.chaves[0].id, atual.chaves[0].RRN, atual.chaves[1].id, atual.chaves[1].RRN, atual.chaves[2].id, atual.chaves[2].RRN,
        atual.desc[0], atual.desc[1], atual.desc[2], atual.desc[3]);
    }
    else{
        printf("NO ATUAL:\ntipo: '%c' | nro_chaves: %d\n(%d,%lli) (%d,%lli) (%d,%lli)\n[%d] [%d] [%d] [%d]\n\n", 
        atual.tipo, atual.nro_chaves, 
        atual.chaves[0].id, atual.chaves[0].byteOffset, atual.chaves[1].id, atual.chaves[1].byteOffset,
        atual.chaves[2].id, atual.chaves[2].byteOffset,
        atual.desc[0], atual.desc[1], atual.desc[2], atual.desc[3]);
    }
}


int insercao_teste(FILE *stream, Cabecalho_b *cab, int RRN_atual, Indice chave, int *RRN_promo_r, Indice *chave_promo, char tipo){
    if (RRN_atual == -1){
        //printf("----no folha voltando\n");
        *chave_promo = chave;
        *RRN_promo_r = -1;
        return 1;
    }
    else{
        int tamNo = tipo == '1' ? TAM_ARVB1 : TAM_ARVB2;
        fseek(stream, (RRN_atual + 1)*tamNo, SEEK_SET);
        Indice_b atual = ler_indice_b(stream, tipo);

        int pos = 0;
        while (pos < atual.nro_chaves - 1 && chave.id > atual.chaves[pos].id)
            pos++;

        if (atual.chaves[pos].id == chave.id)
            return -1;

        if (pos == 0 && chave.id < atual.chaves[pos].id) pos = -1;
        
        int P_B_RRN;
        Indice P_B_KEY;
        int retorno = insercao_teste(stream, cab, atual.desc[pos+1], chave, &P_B_RRN, &P_B_KEY, tipo);
        
        if (retorno == 0 || retorno == -1)
            return retorno;
        else{ 
            //printf("--- no atual a ser promovido\n");
            //mostrar_no(atual, tipo);
            // tem espaco na pagina atual -> inserir na posicao adequada
            if (atual.nro_chaves < ORDEM-1){
                //printf("----tem espaco\n");

                // encontrar posicao e inserir P_B_KEY E P_B_RRN em atual
                int pos = atual.nro_chaves - 1;
                while (pos >= 0 && P_B_KEY.id < atual.chaves[pos].id){
                    atual.chaves[pos+1] = atual.chaves[pos];
                    atual.desc[pos+2] = atual.desc[pos+1];
                    pos--;
                }
                atual.chaves[pos+1] = P_B_KEY;
                atual.desc[pos+2] = P_B_RRN;
                atual.nro_chaves++;

                fseek(stream, (RRN_atual + 1)*tamNo, SEEK_SET);
                escrever_indice_b(stream, atual, tipo);

                //printf("--- no atual ja promovido\n");
                //mostrar_no(atual, tipo);

                return 0;
            }
            else{
                //printf("----nao tem espaco\n");

                Indice_b newpage = criar_no_teste();

                split_teste(cab, P_B_KEY, P_B_RRN, &atual, chave_promo, RRN_promo_r, &newpage, tipo);
            
                fseek(stream, (RRN_atual + 1)*tamNo, SEEK_SET);
                escrever_indice_b(stream, atual, tipo);

                fseek(stream, (*RRN_promo_r + 1)*tamNo, SEEK_SET);
                escrever_indice_b(stream, newpage, tipo);

                //printf("----no esquerda\n");
                //mostrar_no(atual, tipo);
                //printf("----no direita\n");
                //mostrar_no(newpage, tipo);

                return 1;
            }

        }
    }
}

void split_teste(Cabecalho_b *cab, Indice i_key, int i_RRN, Indice_b *atual, Indice *promo_key, int *promo_r_child, Indice_b *newpage, char tipo){
    // copiar todas as chaves e rrns da pagina atual
    Indice chaves[ORDEM];
    int descs[ORDEM+1];
    for (int i = 0; i < ORDEM-1; i++){
        chaves[i] = atual->chaves[i];
        descs[i] = atual->desc[i];
    }
    descs[ORDEM-1] = atual->desc[ORDEM-1];
    descs[ORDEM] = -1;

    // inserir i_key e i_rrn no lugar correto
    int i = ORDEM - 2;
    while (i >= 0 && i_key.id < chaves[i].id){
        chaves[i+1] = chaves[i];
        descs[i+2] = descs[i+1];
        i--;
    }
    chaves[i+1] = i_key;
    descs[i+2] = i_RRN;

    // promokey valor do meio, promo r valor da nova pagina
    *promo_key = chaves[ORDEM/2];
    *promo_r_child = cab->proxRRN;
    cab->proxRRN++;
    cab->nroNos++;

    char tipoNo = atual->tipo;
    *atual = criar_no_teste(); // zerar o no

    atual->nro_chaves = 0;
    for (int i = 0; i < ORDEM/2; i++){
        atual->chaves[i] = chaves[i];
        atual->desc[i] = descs[i];
        atual->nro_chaves++;
    }
    atual->desc[atual->nro_chaves] = descs[(ORDEM/2)];

    newpage->nro_chaves = 0;
    for (int i = ORDEM/2 + 1; i < ORDEM; i++){
        newpage->chaves[newpage->nro_chaves] = chaves[i];
        newpage->desc[newpage->nro_chaves] = descs[i];
        newpage->nro_chaves++;
    }
    newpage->desc[newpage->nro_chaves] = descs[ORDEM];

    if (tipoNo == '0'){
        if (descs[0] == -1){
            atual->tipo = '2';
            newpage->tipo = '2';
        }
        else{
            atual->tipo = '1';
            newpage->tipo = '1';
        }
    }
    else{
        atual->tipo = tipoNo;
        newpage->tipo = tipoNo;
    }
}

Indice_b criar_no_teste(){
    Indice_b ind;
    ind.tipo = '2';
    ind.nro_chaves = 0;
    for (int i = 0; i < ORDEM-1; i++){
        ind.desc[i] = -1;
        ind.chaves[i].id = -1;
        ind.chaves[i].RRN = -1;
        ind.chaves[i].byteOffset = -1;
    }
    ind.desc[ORDEM-1] = -1;
    return ind;
}

void driver(FILE *stream, Cabecalho_b *c, Indice key, char tipo){
    int tamNo = tipo == '1' ? TAM_ARVB1 : TAM_ARVB2;

    if (c->noRaiz == -1){
        Indice_b raiz = criar_no_teste();
        raiz.tipo = '0';
        raiz.chaves[0] = key;
        raiz.nro_chaves++;

        c->noRaiz = c->proxRRN;
        c->proxRRN++;
        c->nroNos++;

        fseek(stream, (c->noRaiz + 1)*tamNo, SEEK_SET);
        escrever_indice_b(stream, raiz, tipo);
    }
    else{
        Indice promo_key;
        int promo_r_child;

        int retorno = insercao_teste(stream, c, c->noRaiz, key, &promo_r_child, &promo_key, tipo);
        if (retorno == 1){
            Indice_b novaRaiz = criar_no_teste();
            novaRaiz.chaves[0] = promo_key;
            novaRaiz.desc[0] = c->noRaiz;
            novaRaiz.desc[1] = promo_r_child;
            novaRaiz.nro_chaves++;
            novaRaiz.tipo = '0';
            c->noRaiz = c->proxRRN;
            c->proxRRN++;
            c->nroNos++;
            fseek(stream, (c->noRaiz + 1)*tamNo, SEEK_SET);
            escrever_indice_b(stream, novaRaiz, tipo);
        }
    }
}