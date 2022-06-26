#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veiculo.h"
#include "utils.h"
#include "cabecalho.h"
#include "indice.h"

void ler_campo(veiculo *f){
    char *campo;
    scanf("%ms", &campo);
    if (!strcmp(campo, "id")){
        scanf("%d ", &f->id);
    }
    else if (!strcmp(campo, "ano")){
        scanf("%d ", &f->ano);
    }
    else if (!strcmp(campo, "quantidade")){
        scanf("%d ", &f->qtt);
    }
    else if (!strcmp(campo, "sigla")){
        char *sigla = scan_quote_string();
        strncpy(f->sigla, sigla, 2);
        free(sigla);
    }
    else if (!strcmp(campo, "cidade")){
        f->cidade = scan_quote_string();
    }
    else if (!strcmp(campo, "marca")){
        f->marca = scan_quote_string();
    }
    else if (!strcmp(campo, "modelo")){
        f->modelo = scan_quote_string();
    }
    free(campo);
}

void ler_novo_campo(veiculo *valores, veiculo *campos){
    char *campo, *valor;
    scanf("%ms", &campo);
    //printf("campo lido: %s\n", campo);
    if (!strcmp(campo, "id")){
        campos->id = 1;
        scanf("%ms", &valor);
        if (strcmp(valor, "NULO") != 0) valores->id = atoi(valor);
    }
    else if (!strcmp(campo, "ano")){
        campos->ano = 1;
        scanf("%ms", &valor);
        if (strcmp(valor, "NULO") != 0) valores->ano = atoi(valor);
    }
    else if (!strcmp(campo, "qtt")){
        campos->qtt = 1;
        scanf("%ms", &valor);
        if (strcmp(valor, "NULO") != 0) valores->qtt = atoi(valor);
    }
    else if (!strcmp(campo, "sigla")){
        strncpy(campos->sigla, "11", 2);
        valor = scan_quote_string();
        if (strcmp(valor, "") != 0) strncpy(valores->sigla, valor, 2);
    }
    else if (!strcmp(campo, "cidade")){
        campos->cidade = malloc(2 * sizeof(char));
        strcpy(campos->cidade, "1");
        valor = scan_quote_string();
        if (strcmp(valor, "") != 0){
            valores->cidade = realloc(valores->cidade, (strlen(valor) * sizeof(char)) + 1);
            strcpy(valores->cidade, valor);
        }
    }
    else if (!strcmp(campo, "marca")){
        campos->marca = malloc(2 * sizeof(char));
        strcpy(campos->marca, "1");
        valor = scan_quote_string();
        if (strcmp(valor, "") != 0){
            valores->marca = realloc(valores->marca, (strlen(valor) * sizeof(char)) + 1);
            strcpy(valores->marca, valor);
        }
    }
    else if (!strcmp(campo, "modelo")){
        campos->modelo = malloc(2 * sizeof(char));
        strcpy(campos->modelo, "1");
        valor = scan_quote_string();
        if (strcmp(valor, "") != 0){
            valores->modelo = realloc(valores->modelo, (strlen(valor) * sizeof(char)) + 1);
            strcpy(valores->modelo, valor);
        }
    }
    free(campo);
    free(valor);
}

int main(void){
    int cmd;
    scanf("%d ", &cmd);
    switch (cmd){
        /* funcionalidade 1:
        leitura de varios registros em arquivo csv e escrita em arquivo binario */
        case 1: {
            // leitura do comando e abertura dos arquivos
            char *tipo, *csvname, *binname;
            scanf("%ms %ms %ms", &tipo, &csvname, &binname);
            FILE *csv = fopen(csvname, "r");
            FILE *bin = fopen(binname, "wb");
            if (csv == NULL){
                printf("Falha no processamento do arquivo.\n");
                break;
            }

            // registro de cabecalho inicial
            cabecalho rc = {'0', '0', '1', '2',
                            -1, 0, 0,
                            -1, 190,
                            "LISTAGEM DA FROTA DOS VEICULOS NO BRASIL",
                            "CODIGO IDENTIFICADOR: ",
                            "ANO DE FABRICACAO: ",
                            "QUANTIDADE DE VEICULOS: ",
                            "ESTADO: ",
                            "NOME DA CIDADE: ",
                            "MARCA DO VEICULO: ",
                            "MODELO DO VEICULO: "};

            // se tipo selecionado for "tipo1"
            if (strcmp(tipo, "tipo1") == 0){
                escrever_cabecalho(rc, bin, '1');
                linebreak(csv);

                char c;
                while ((c = fgetc(csv)) != EOF){
                    ungetc(c, csv);
                    veiculo v = ler_veiculo_csv(csv);

                    // escrita no arquivo bin
                    char removido = '0';
                    fwrite(&removido, 1, 1, bin);
                    int prox = -1;
                    fwrite(&prox, 4, 1, bin);
                    escrever_veiculo(v, bin);

                    // preencher resto do registro com lixo
                    int tamRegistro = 1 + 4 + calcular_tamanho(v);
                    char lixo = '$';
                    while (tamRegistro < 97){
                        fwrite(&lixo, 1, 1, bin);
                        tamRegistro++;
                    }

                    rc.proxRRN += 1;
                    desalocar_veiculo(v);
                }
                // atualizacao do cabecalho no arquivo binario
                fseek(bin, 174, SEEK_SET);
                fwrite(&rc.proxRRN, 4, 1, bin);
            }
            // se tipo selecionado for "tipo2"
            else if (strcmp(tipo, "tipo2") == 0){
                escrever_cabecalho(rc, bin, '2');
                linebreak(csv);

                char c;
                while ((c = fgetc(csv)) != EOF){
                    ungetc(c, csv);
                    veiculo v = ler_veiculo_csv(csv);

                    // escrita no arquivo bin
                    int tamRegistro = 8 + calcular_tamanho(v);
                    char removido = '0';
                    fwrite(&removido, 1, 1, bin);
                    fwrite(&tamRegistro, 4, 1, bin);
                    long int prox = -1;
                    fwrite(&prox, 8, 1, bin);
                    escrever_veiculo(v, bin);
                    desalocar_veiculo(v);

                    rc.proxByteOffset += 1 + 4 + tamRegistro;
                }
                // atualizacao do cabecalho no arquivo binario
                fseek(bin, 178, SEEK_SET);
                fwrite(&rc.proxByteOffset, 8, 1, bin);
            }
            // atualizacao do cabecalho no arquivo binario
            rc.status = '1';
            fseek(bin, 0, SEEK_SET);
            fwrite(&rc.status, 1, 1, bin);

            fclose(csv);
            fclose(bin);

            binarioNaTela(binname);
            free(binname);
            free(csvname);
            free(tipo);
        }
        break;

        /* funcionalidade 2:
        recuperacao e exibicao de todos os registros armazenados em um arquivo */
        case 2: {
            // leitura do comando e abertura do arquivo
            char *tipo, *binname;
            scanf("%ms %ms", &tipo, &binname);
            FILE *bin = fopen(binname, "rb");
            if (bin == NULL){
                printf("Falha no processamento do arquivo.\n");
                free(tipo);
                free(binname);
                fclose(bin);
                break;
            }

            int qtd = 0;
            // se tipo selecionado for "tipo1"
            if (strcmp(tipo, "tipo1") == 0){
                cabecalho rc = ler_cabecalho(bin, '1');
                if (rc.status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    free(tipo);
                    free(binname);
                    fclose(bin);
                    break;
                }

                // leitura e exibicao dos registros
                char removido;
                while (fread(&removido, 1, 1, bin), !feof(bin)){
                    // verificar se registro foi removido
                    if (removido == '1'){
                        fseek(bin, 96, SEEK_CUR);
                        continue;
                    }

                    qtd++;
                    fseek(bin, 4, SEEK_CUR);
                    veiculo v = ler_veiculo(bin, 97 - 1 - 4);
                    printf("%d\n", v.id);
                    mostrar_veiculo(v);
                    desalocar_veiculo(v);
                }
            }
            // se tipo selecionado for "tipo2"
            else if (strcmp(tipo, "tipo2") == 0){
                cabecalho rc = ler_cabecalho(bin, '2');
                if (rc.status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    free(tipo);
                    free(binname);
                    fclose(bin);
                    break;
                }

                // leitura e exibicao dos registros
                char removido;
                while (fread(&removido, 1, 1, bin), !feof(bin)){
                    // verificar se registro foi removido
                    int tamRegistro;
                    fread(&tamRegistro, 4, 1, bin);
                    if (removido == '1'){
                        fseek(bin, tamRegistro, SEEK_CUR);
                        continue;
                    }

                    qtd++;
                    fseek(bin, 8, SEEK_CUR);
                    veiculo v = ler_veiculo(bin, tamRegistro - 8);
                    mostrar_veiculo(v);
                    desalocar_veiculo(v);
                }
            }
            if (qtd == 0) printf("Registro inexistente.\n");

            free(tipo);
            free(binname);
            fclose(bin);
        }
        break;

        /* Funcionalidade 3:
        recuperacao de todos os registros que satisfazem criterios de busca */
        
        case 3:{
            // leitura do comando
            char *tipo, *binname;
            int nCampos;
            scanf("%ms %ms %d", &tipo, &binname, &nCampos);
            if(strcmp(tipo, "tipo1") && strcmp(tipo, "tipo2")){
                printf("Falha no processamento do arquivo.\n");
                free(binname);
                free(tipo);
                break;
            }

            // leitura dos criterios de busca
            veiculo f = {-1, -1, -1, "$$", NULL, NULL, NULL};
            while (nCampos--){
                char *campo;
                scanf("%ms", &campo);
                if (!strcmp(campo, "id")){
                    scanf("%d", &f.id);
                    linebreak(stdin);
                }
                else if (!strcmp(campo, "ano")){
                    scanf("%d", &f.ano);
                    linebreak(stdin);
                }
                else if (!strcmp(campo, "quantidade")){
                    scanf("%d", &f.qtt);
                    linebreak(stdin);
                }
                else if (!strcmp(campo, "sigla")){
                    fgets(f.sigla, 2, stdin);
                    linebreak(stdin);
                }
                else if (!strcmp(campo, "cidade")){
                    f.cidade = scan_quote_string();
                }
                else if (!strcmp(campo, "marca")){
                    f.marca = scan_quote_string();
                }
                else if (!strcmp(campo, "modelo")){
                    f.modelo = scan_quote_string();
                }
                free(campo);
            }

            // abertura do arquivo
            FILE *bin = fopen(binname, "rb");
            free(binname);
            if (bin == NULL){
                printf("Falha no processamento do arquivo.\n");
                desalocar_veiculo(f);
                free(tipo);
                break;
            }

            cabecalho rc = ler_cabecalho(bin, tipo[4]);
            if(rc.status == '0'){
                printf("Falha no processamento do arquivo.\n");
                desalocar_veiculo(f);
                free(tipo);
                fclose(bin);
                break;
            }

            char c;
            char found = 0;
            while ((c = fgetc(bin)) != EOF){
                ungetc(c, bin);
                // ler e verificar se veiculo corresponde aos criterios de busca
                veiculo v = {-1, -1, -1, "$$", NULL, NULL, NULL};
                long int next;
                long int cur = filtrarVeiculo(bin, f, tipo[4], &v, &next);
                if(cur != -1){
                    found = 1;
                    mostrar_veiculo(v);
                }
                desalocar_veiculo(v);
                // posicionar leitura do arquivo no proximo registro
                if (next) fseek(bin, next, SEEK_CUR);
            }
            if(!found) printf("Registro inexistente.\n");

            desalocar_veiculo(f);
            free(tipo);
            fclose(bin);
        }
        break;
        
        /* Funcionalidade 4:
        recuperacao e exibicao de registro a partir de RRN */
        case 4:{
            // leitura do comando e abertura do arquivo
            char *tipo, *binname;
            int RRN;
            scanf("%ms %ms %d", &tipo, &binname, &RRN);
            if(strcmp(tipo, "tipo1")){
                printf("Falha no processamento do arquivo.\n");
                free(binname);
                free(tipo);
                break;
            }
            FILE *bin = fopen(binname, "rb");
            if (bin == NULL){
                printf("Falha no processamento do arquivo.\n");
                free(binname);
                free(tipo);
                break;
            }

            cabecalho rc = ler_cabecalho(bin, '1');
            if (rc.status == '0'){
                printf("Falha no processamento do arquivo.\n");
                fclose(bin);
                free(binname);
                free(tipo);
                break;
            }

            // posicionar a leitura do arquivo no registro desejado
            fseek(bin, RRN * 97, SEEK_CUR); 

            // verificar se registro nao existe ou foi removido
            char removido;
            fread(&removido, 1, 1, bin);
            if (feof(bin) || (removido == '1')){
                printf("Registro inexistente.\n");
                fclose(bin);
                free(binname);
                free(tipo);
                break;
            }

            // leitura e exibicao do registro
            fseek(bin, 4, SEEK_CUR);
            veiculo v = ler_veiculo(bin, 97 - 1 - 4);
            mostrar_veiculo(v);

            desalocar_veiculo(v);
            fclose(bin);
            free(binname);
            free(tipo);
        }
        break;

        /* Funcionalidade 5:
        criacao de arquivo de indice primario simples a partir de arquivo de dados */
        case 5:{
            // leitura do comando e abertura dos arquivos
            char *tipo, *binname, *indname;
            scanf("%ms %ms %ms", &tipo, &binname, &indname);
            FILE *bin = fopen(binname, "rb");
            FILE *ind = fopen(indname, "wb");
            if (bin == NULL || ind == NULL){
                printf("Falha no processamento do arquivo.\n");
                free(tipo);
                free(binname);
                free(indname);
                if (bin != NULL) fclose(bin);
                if (ind != NULL) fclose(ind);
                break;
            }
            
            // verificacao do status do arquivo
            cabecalho rc;
            if (strcmp(tipo, "tipo1") == 0) rc = ler_cabecalho(bin, '1');
            else if (strcmp(tipo, "tipo2") == 0) rc = ler_cabecalho(bin, '2');
            if (rc.status == '0'){
                printf("Falha no processamento do arquivo.\n");
                free(tipo);
                free(binname);
                free(indname);
                fclose(bin);
                fclose(ind);
                break;
            }

            funcionalidade_5(bin, ind, tipo);

            fclose(bin);
            fclose(ind);
            binarioNaTela(indname);
            free(tipo);
            free(binname);
            free(indname);
        }
        break;

        /* Funcionalidade 6:
        remocao logica de um registro de um arquivo de dados */
        case 6:{
            // leitura do comando e abertura dos arquivos
            char *tipo, *binname, *indname;
            int nRemocoes;
            scanf("%ms %ms %ms %d", &tipo, &binname, &indname, &nRemocoes);
            FILE *bin = fopen(binname, "rb+");
            FILE *ind = fopen(indname, "rb");
            if (bin == NULL || ind == NULL){
                printf("Falha no processamento do arquivo.\n");
                free(tipo);
                free(binname);
                free(indname);
                if (bin != NULL) fclose(bin);
                if (ind != NULL) fclose(ind);
                break;
            }

            // verificacao dos status e leitura dos indices
            int qtd_ind = -1;
            Indice *indices = NULL;
            cabecalho rc = ler_cabecalho(bin, tipo[4]);
            // printf("topo inicial: %d\n", rc.topo1);
            if (strcmp(tipo, "tipo1") == 0){
                if (rc.status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    free(tipo);
                    free(binname);
                    free(indname);
                    fclose(bin);
                    fclose(ind);
                    break;
                }
                indices = ler_indices(ind, &qtd_ind, '1');
            } 
            else if (strcmp(tipo, "tipo2") == 0){
                if (rc.status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    free(tipo);
                    free(binname);
                    free(indname);
                    fclose(bin);
                    fclose(ind);
                }
                indices = ler_indices(ind, &qtd_ind, '2');
            }

            // printf("quantidade de indices: %d\n", qtd_ind);
            // mostrar_indices(indices, qtd_ind, '2');
            long int inicio = ftell(bin);
            while (nRemocoes--){
                fseek(bin, inicio, SEEK_SET);
                int nCampos;
                scanf("%d", &nCampos);
                veiculo f = {-1, -1, -1, "$$", NULL, NULL, NULL};
                while (nCampos--){
                    ler_campo(&f);
                }

                long int next;
                long int cur = buscar_veiculo(bin, indices, qtd_ind, f, tipo[4], &next);
                // printf("offset: %ld\n", cur);
                // FILE *bin2 = fopen(binname, "rb");
                // fseek(bin2, cur + 5, SEEK_SET);
                // mostrar_veiculo(ler_veiculo(bin2, 97));
                if(f.id == -1){
                    // int max = 0;
                    while (cur != -1){
                        long end = ftell(bin);
                        // printf("next: %d\noffset: %ld\nend: %ld\n", next, cur, end);
                        // fseek(bin2, cur + 5, SEEK_SET);
                        // mostrar_veiculo(ler_veiculo(bin2, 97));
                        remover_veiculo(bin, cur, tipo[4], &rc);
                        // fseek(bin, cur, SEEK_SET);
                        // char rem = '1';
                        // fwrite(&rem, 1, 1, bin);
                        // if(tipo[4] == '1'){
                        //     fwrite(&rc.topo1, 4, 1, bin);
                        //     rc.topo1 = (cur - 182) / 97;
                        // }
                        // else if(tipo[4] == '2'){
                        //     int tam;
                        //     fread(&tam, 4, 1, bin);
                        //     // printf("tamanho: %d\n", tam);
                        //     long prox = ftell(bin);
                        //     long ant = -1;
                        //     long offset;
                        //     // printf("topo: %d\n", rc.topo2);
                        //     if(rc.topo2 != -1){
                        //         fseek(bin, rc.topo2 + 1, SEEK_SET);
                        //         int tam2;
                        //         fread(&tam2, 4, 1, bin);
                        //         // printf("%d\n", tam2);
                        //         while(tam < tam2){
                        //             ant = ftell(bin);
                        //             fread(&offset, 8, 1, bin);
                        //             fseek(bin, offset + 1, SEEK_SET);
                        //             fread(&tam2, 4, 1, bin);
                        //         }
                        //     }
                        //     if(ant == -1){
                        //         fseek(bin, prox, SEEK_SET);
                        //         fwrite(&rc.topo2, 8, 1, bin);
                        //         rc.topo2 = cur;
                        //     }
                        //     else{
                        //         fseek(bin, ant, SEEK_SET);
                        //         fwrite(&cur, 8, 1, bin);
                        //         fseek(bin, prox, SEEK_SET);
                        //         fwrite(&offset, 8, 1, bin);
                        //     }
                        // }
                        // rc.nroRegRem++;
                        fseek(bin, end + next, SEEK_SET);
                        cur = buscar_veiculo(bin, indices, qtd_ind, f, tipo[4], &next);
                        // max++;
                    }
                }
                else if(cur != -1){
                    remover_veiculo(bin, cur, tipo[4], &rc);
                    // fseek(bin, cur, SEEK_SET);
                    // char rem = '1';
                    // fwrite(&rem, 1, 1, bin);
                    // if(tipo[4] == '1'){
                    //     fwrite(&rc.topo1, 4, 1, bin);
                    //     rc.topo1 = (cur - 182) / 97;
                    // }
                    // else if(tipo[4] == '2'){
                    //     int tam;
                    //     fread(&tam, 4, 1, bin);
                    //     // printf("tamanho: %d\n", tam);
                    //     long prox = ftell(bin);
                    //     long ant = -1;
                    //     long offset;
                    //     // printf("topo: %d\n", rc.topo2);
                    //     if(rc.topo2 != -1){
                    //         fseek(bin, rc.topo2 + 1, SEEK_SET);
                    //         int tam2;
                    //         fread(&tam2, 4, 1, bin);
                    //         // printf("%d\n", tam2);
                    //         while(tam < tam2){
                    //             ant = ftell(bin);
                    //             fread(&offset, 8, 1, bin);
                    //             fseek(bin, offset + 1, SEEK_SET);
                    //             fread(&tam2, 4, 1, bin);
                    //         }
                    //     }
                    //     if(ant == -1){
                    //         fseek(bin, prox, SEEK_SET);
                    //         fwrite(&rc.topo2, 8, 1, bin);
                    //         rc.topo2 = cur;
                    //     }
                    //     else{
                    //         fseek(bin, ant, SEEK_SET);
                    //         fwrite(&cur, 8, 1, bin);
                    //         fseek(bin, prox, SEEK_SET);
                    //         fwrite(&offset, 8, 1, bin);
                    //     }
                    // }
                    // rc.nroRegRem++;
                }
                // fclose(bin2);
                // teste
                // printf("id: %d\n", f.id);
                // printf("sigla: %s\n", f.sigla);
                // mostrar_veiculo(f);
                //

                // printf("%d\n", buscar_veiculo(bin, indices, qtd_ind, f, tipo[4]));
                //while (buscar_veiculo(FILE *bin, FILE, veiculo f, char tipo, veiculo *v))
            }
            fseek(bin, 0, SEEK_SET);
            // printf("topo final: %d\n", rc.topo1);
            escrever_cabecalho(rc, bin, tipo[4]);
            fclose(ind);
            ind = fopen(indname, "wb");
            funcionalidade_5(bin, ind, tipo);
            fclose(bin);
            binarioNaTela(binname);
            binarioNaTela(indname);
        }
        break;

        /* Funcionalidade 7:
        insercao de um novo registro em um arquivo de dados */
        case 7:{
            // leitura do comando e abertura dos arquivos
            char *tipo, *binname, *indname;
            int nInsercoes;
            scanf("%ms %ms %ms %d", &tipo, &binname, &indname, &nInsercoes);
            FILE *bin = fopen(binname, "rb+");
            FILE *ind = fopen(indname, "rb+");
            if (bin == NULL || ind == NULL){
                printf("Falha no processamento do arquivo.\n");
                free(tipo);
                free(binname);
                free(indname);
                if (bin != NULL) fclose(bin);
                if (ind != NULL) fclose(ind);
                break;
            }

            char status;
            fread(&status, sizeof(char), 1, ind);

            // se tipo selecionado for "tipo1"
            if (strcmp(tipo, "tipo1") == 0){
                // verificacao dos status dos arquivos
                cabecalho rc = ler_cabecalho(bin, '1');
                if (rc.status == '0' || status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    free(tipo);
                    free(binname);
                    free(indname);
                    fclose(bin);
                    fclose(ind);
                    break;
                }

                // indicar que arquivos de dados estao sendo alterados
                status = '0';
                fseek(ind, 0, SEEK_SET);
                fwrite(&status, sizeof(char), 1, ind);
                fseek(bin, 0, SEEK_SET);
                fwrite(&status, sizeof(char), 1, bin);

                // leitura e insercao dos veiculos
                while (nInsercoes--){
                    veiculo v = ler_novo_veiculo(stdin);

                    char removido = '0';
                    int prox = -1;
                    // nao existem registros removidos -> inserir no prox RRN disponivel
                    if (rc.topo1 == -1){
                        fseek(bin, 182 + (rc.proxRRN * 97), SEEK_SET);
                        fwrite(&removido, sizeof(char), 1, bin);
                        fwrite(&prox, sizeof(int), 1, bin);
                        escrever_veiculo(v, bin);
                        rc.proxRRN++;
                    }
                    // existe registro removido -> inserir no RRN topo da pilha de removidos
                    else{
                        fseek(bin, 182 + (rc.topo1 * 97), SEEK_SET);
                        fwrite(&removido, sizeof(char), 1, bin);
                        fread(&rc.topo1, sizeof(int), 1, bin); // novo topo: proximo do topo atual
                        fseek(bin, -4, SEEK_CUR);
                        fwrite(&prox, sizeof(int), 1, bin);
                        escrever_veiculo(v, bin);
                        rc.nroRegRem--;
                    }

                    // preencher resto do registro com lixo
                    int tamRegistro = 1 + 4 + calcular_tamanho(v);
                    char lixo = '$';
                    while (tamRegistro < 97){
                        fwrite(&lixo, sizeof(char), 1, bin);
                        tamRegistro++;
                    }

                    desalocar_veiculo(v);
                }

                // atualizar arquivo de indices
                fseek(bin, 182, SEEK_SET);
                fclose(ind);
                ind = fopen(indname, "wb");
                funcionalidade_5(bin, ind, tipo);

                // atualizar cabecalho do registro de dados
                fseek(bin, 0, SEEK_SET);
                escrever_cabecalho(rc, bin, '1');
            } 
            // se tipo selecionado for "tipo2"
            else if (strcmp(tipo, "tipo2") == 0){
                 // verificacao dos status dos arquivos
                cabecalho rc = ler_cabecalho(bin, '2');
                if (rc.status == '0' || status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    free(tipo);
                    free(binname);
                    free(indname);
                    fclose(bin);
                    fclose(ind);
                    break;
                }

                // indicar que arquivos de dados estao sendo alterados
                status = '0';
                fseek(ind, 0, SEEK_SET);
                fwrite(&status, sizeof(char), 1, ind);
                fseek(bin, 0, SEEK_SET);
                fwrite(&status, sizeof(char), 1, bin);

                // leitura e insercao dos veiculos
                while (nInsercoes--){
                    veiculo v = ler_novo_veiculo(stdin);

                    // int tamTopo = -1;
                    // char removido = '0';
                    // long prox = -1;
                    // if (rc.topo2 != -1){
                    //     fseek(bin, rc.topo2 + 1, SEEK_SET);
                    //     fread(&tamTopo, sizeof(int), 1, bin);
                    // }
                    // int tamRegistro = 8 + calcular_tamanho(v);
                    // // nao existem registros removidos ou espaco insuficiente -> escrever no fim do arquivo
                    // if (rc.topo2 == -1 || tamRegistro > tamTopo){
                    //     fseek(bin, rc.proxByteOffset, SEEK_SET);
                    //     fwrite(&removido, sizeof(char), 1, bin);
                    //     fwrite(&tamRegistro, sizeof(int), 1, bin);
                    //     fwrite(&prox, sizeof(long), 1, bin);
                    //     escrever_veiculo(v, bin);
                    //     rc.proxByteOffset += 1 + 4 + tamRegistro;
                    // }
                    // // existe registro removido com espaco suficiente -> escrever no byteOffset
                    // else{
                    //     fseek(bin, rc.topo2, SEEK_SET);
                    //     fwrite(&removido, sizeof(char), 1, bin);
                    //     fseek(bin, 4, SEEK_CUR);
                    //     fread(&rc.topo2, sizeof(long), 1, bin);
                    //     fseek(bin, -8, SEEK_CUR);
                    //     fwrite(&prox, sizeof(long), 1, bin);
                    //     escrever_veiculo(v, bin);
                    //     rc.nroRegRem--;
                        
                    //     // preencher resto do registro com lixo
                    //     char lixo = '$';
                    //     while (tamRegistro < tamTopo){
                    //         fwrite(&lixo, sizeof(char), 1, bin);
                    //         tamRegistro++;
                    //     }
                    // }
                    inserir_veiculo(&rc, bin, v);
                    desalocar_veiculo(v);
                }

                // atualizar arquivo de indices
                fseek(bin, 190, SEEK_SET);
                fclose(ind);
                ind = fopen(indname, "wb");
                funcionalidade_5(bin, ind, tipo);

                // atualizar cabecalho do registro de dados
                fseek(bin, 0, SEEK_SET);
                escrever_cabecalho(rc, bin, '2');
            }

            fclose(bin);
            fclose(ind);
            binarioNaTela(binname);
            binarioNaTela(indname);
            free(tipo);
            free(binname);
            free(indname);
        }
        break;

        /* Funcionalidade 8:
        atualizacao de registros de um arquivo de dados */
        case 8:{
            char *tipo, *binname, *indname;
            int nAtualizacoes;
            scanf("%ms %ms %ms %d", &tipo, &binname, &indname, &nAtualizacoes);
            FILE *bin = fopen(binname, "rb+");
            FILE *ind = fopen(indname, "rb+");
            if (bin == NULL || ind == NULL){
                printf("Falha no processamento do arquivo.\n");
                free(tipo);
                free(binname);
                free(indname);
                if (bin != NULL) fclose(bin);
                if (ind != NULL) fclose(ind);
                break;
            }

            char status;
            fread(&status, sizeof(char), 1, ind);

            // se tipo selecionado for "tipo1"
            if (strcmp(tipo, "tipo1") == 0){
                // verificacao dos status dos arquivos
                cabecalho rc = ler_cabecalho(bin, '1');
                if (rc.status == '0' || status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    free(tipo);
                    free(binname);
                    free(indname);
                    fclose(bin);
                    fclose(ind);
                    break;
                }

                long inicio = ftell(bin);
                while (nAtualizacoes--){
                    fseek(bin, inicio, SEEK_SET);
                    veiculo filtro = {-1, -1, -1, "$$", NULL, NULL, NULL}; // criterios de busca
                    veiculo valores = {-1, -1, -1, "$$", NULL, NULL, NULL}; // valores que devem ser atualizados
                    veiculo campos = {-1, -1, -1, "$$", NULL, NULL, NULL}; // indica os campos que devem ser atualizados

                    int nCamposBusca, nCamposAtualiza;
                    scanf("%d ", &nCamposBusca);
                    while (nCamposBusca--){
                        ler_campo(&filtro);
                    }
                    scanf("%d ", &nCamposAtualiza);
                    while (nCamposAtualiza--){
                        ler_novo_campo(&valores, &campos);
                    }

                    int qtd_ind;
                    fseek(ind, 0, SEEK_SET);
                    Indice *indices = ler_indices(ind, &qtd_ind, '1');
                    // mostrar_indices(indices, qtd_ind, '1');
                    // mostrar_veiculo(filtro);
                    // printf("primeiro %d\n", qtd_ind);
                    long int cur, next;
                    while (cur = buscar_veiculo(bin, indices, qtd_ind, filtro, tipo[4], &next), cur != -1){
                        // printf("segundo\n");
                        fseek(bin, cur + 5, SEEK_SET);
                        veiculo v = ler_veiculo(bin, 97);
                        // printf("terceiro\n");
                        // mostrar_veiculo(v);
                        atualizar_veiculo_1(&v, &valores, &campos);
                        fseek(bin, cur + 5, SEEK_SET);
                        escrever_veiculo(v, bin);

                        // preencher resto do registro com lixo
                        char lixo = '$';
                        int tamRegistro = 1 + 4 + calcular_tamanho(v);
                        while (tamRegistro < 97){
                            fwrite(&lixo, sizeof(char), 1, bin);
                            tamRegistro++;
                        }

                        desalocar_veiculo(v);
                        if(filtro.id != -1) break;
                    }

                    desalocar_veiculo(filtro);
                    desalocar_veiculo(valores);
                    desalocar_veiculo(campos);
                }

                // atualizar arquivo de indices
                fseek(bin, 182, SEEK_SET);
                fclose(ind);
                ind = fopen(indname, "wb");
                funcionalidade_5(bin, ind, tipo);

                fclose(bin);
                fclose(ind);
                binarioNaTela(binname);
                binarioNaTela(indname);
                free(tipo);
                free(binname);
                free(indname);

            }
            // se tipo selecionado for "tipo2"
            else if (strcmp(tipo, "tipo2") == 0){
                // verificacao dos status dos arquivos
                cabecalho rc = ler_cabecalho(bin, '2');
                if (rc.status == '0' || status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    free(tipo);
                    free(binname);
                    free(indname);
                    fclose(bin);
                    fclose(ind);
                    break;
                }

                long inicio = ftell(bin);
                while (nAtualizacoes--){
                    fseek(bin, inicio, SEEK_SET);
                    veiculo filtro = {-1, -1, -1, "$$", NULL, NULL, NULL}; // criterios de busca
                    veiculo valores = {-1, -1, -1, "$$", NULL, NULL, NULL}; // valores que devem ser atualizados
                    veiculo campos = {-1, -1, -1, "$$", NULL, NULL, NULL}; // indica os campos que devem ser atualizados

                    int nCamposBusca, nCamposAtualiza;
                    scanf("%d ", &nCamposBusca);
                    while (nCamposBusca--){
                        ler_campo(&filtro);
                    }
                    scanf("%d ", &nCamposAtualiza);
                    while (nCamposAtualiza--){
                        ler_novo_campo(&valores, &campos);
                    }

                    // printf("primeiro\n");
                    int qtd_ind;
                    fseek(ind, 0, SEEK_SET);
                    // printf("segundo\n");
                    Indice *indices = ler_indices(ind, &qtd_ind, '2');
                    // printf("terceiro\n");
                    // mostrar_indices(indices, qtd_ind, '2');
                    // printf("quarto\n");
                    long int cur, next;
                    while (cur = buscar_veiculo(bin, indices, qtd_ind, filtro, tipo[4], &next), cur != -1){
                        long end = ftell(bin);
                        // printf("quinto\n");
                        fseek(bin, cur + 1, SEEK_SET);
                        // printf("sexto\n");
                        int tam;
                        fread(&tam, 4, 1, bin);
                        // printf("setimo\n");
                        fseek(bin, 8, SEEK_CUR);
                        // printf("oitavo\n");
                        veiculo v = ler_veiculo(bin, tam - 8);
                        // printf("nono\n");
                        // mostrar_veiculo(v);
                        // deve funcionar
                        atualizar_veiculo_1(&v, &valores, &campos);
                        int tamreg = calcular_tamanho(v) + 8;
                        if(tamreg <= tam){
                            fseek(bin, cur + 13, SEEK_SET);
                            escrever_veiculo(v, bin);
                            char lixo = '$';
                            while (tamreg < tam){
                                fwrite(&lixo, sizeof(char), 1, bin);
                                tamreg++;
                            }
                        }
                        else{
                            remover_veiculo(bin, cur, '2', &rc);
                            inserir_veiculo(&rc, bin, v);
                        }
                        // if novotamanho <= tamanho : escreve ai mesmo
                        // else remocao + insercao

                        // preencher resto do registro com lixo -- talvez tenha isos mesmo se inserir aqui

                        desalocar_veiculo(v);
                        if(filtro.id != -1) break;
                        // printf("offset: %d\n", end + next);
                        fseek(bin, end + next, SEEK_SET);
                    }
                    // printf("decimo\n");

                    desalocar_veiculo(filtro);
                    desalocar_veiculo(valores);
                    desalocar_veiculo(campos);
                }

                // atualizar arquivo de indices
                fseek(bin, 190, SEEK_SET);
                fclose(ind);
                ind = fopen(indname, "wb");
                funcionalidade_5(bin, ind, tipo);

                // atualizar cabecalho do registro de dados
                fseek(bin, 0, SEEK_SET);
                escrever_cabecalho(rc, bin, '2');

                fclose(bin);
                fclose(ind);
                binarioNaTela(binname);
                binarioNaTela(indname);
                free(tipo);
                free(binname);
                free(indname);

            }
        }
        break;
    }
    return 0;
}