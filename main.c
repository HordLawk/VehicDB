#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veiculo.h"
#include "utils.h"
#include "cabecalho.h"
#include "indice.h"

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
            while ((c = fgetc(bin)) != EOF){
                ungetc(c, bin);
                // ler e verificar se veiculo corresponde aos criterios de busca
                veiculo v = {-1, -1, -1, "$$", NULL, NULL, NULL};
                int next = filtrarVeiculo(bin, f, tipo[4], &v);
                desalocar_veiculo(v);
                // posicionar leitura do arquivo no proximo registro
                if (next) fseek(bin, next, SEEK_CUR);
            }

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

            char status = '0';
            fwrite(&status, sizeof(char), 1, ind);

            // se tipo selecionado for "tipo1"
            if (strcmp(tipo, "tipo1") == 0){
                cabecalho rc = ler_cabecalho(bin, '1');
                if (rc.status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    free(tipo);
                    free(binname);
                    free(indname);
                    fclose(bin);
                    fclose(ind);
                    break;
                }

                Indice *indices;
                int qtd_ind = 0;

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
                //mostrar_indices(indices,qtd_ind, '1');
                escrever_indices(indices, qtd_ind, ind, '1');

                free(indices);
            }
            // se tipo selecionado for "tipo2"
            else if (strcmp(tipo, "tipo2") == 0){
                cabecalho rc = ler_cabecalho(bin, '2');
                if (rc.status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    free(tipo);
                    free(binname);
                    free(indname);
                    fclose(bin);
                    fclose(ind);
                    break;
                }

                Indice *indices;
                int qtd_ind = 0;

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

                    long proxByteOffset = ftell(bin) - 1 - 8 - 4; // eu acho
                    // criar indice que representa registro lido
                    Indice *aux = realloc(indices, ((qtd_ind + 1) * sizeof(Indice)));
                    indices = aux;
                    indices[qtd_ind].byteOffset = proxByteOffset;
                    fread(&indices[qtd_ind].id, sizeof(int), 1, bin);
                    qtd_ind++;

                    // mover ponteiro para proximo registro
                    fseek(bin, tamRegistro - 4 - 8, SEEK_CUR); 
                }

                ordenar_indices(indices, qtd_ind, '2');
                mostrar_indices(indices,qtd_ind, '2');
                escrever_indices(indices, qtd_ind, ind, '2');
            }

            status = '1';
            fseek(ind, 0, SEEK_SET);
            fwrite(&status, sizeof(char), 1, ind);

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
            


        }
        break;

        /* Funcionalidade 7:
        insercao de um novo registro em um arquivo de dados */
        case 7:{
            
        }
        break;

        /* Funcionalidade 8:
        atualizacao de registros de um arquivo de dados */
        case 8:{

        }
        break;
    }
    return 0;
}