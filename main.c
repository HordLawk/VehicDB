#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veiculo.h"
#include "utils.h"
#include "cabecalho.h"
#include "indice.h"

int main(void){
    char *tipo = NULL, *csvname = NULL, *binname = NULL, *indname = NULL;
    int cmd;
    scanf("%d ", &cmd);
    switch (cmd){
        /* funcionalidade 1:
        leitura de varios registros em arquivo csv e escrita em arquivo binario */
        case 1: {
            // leitura do comando e abertura dos arquivos
            scanf("%ms %ms %ms", &tipo, &csvname, &binname);
            FILE *csv = fopen(csvname, "r");
            if (csv == NULL){
                printf("Falha no processamento do arquivo.\n");
                break;
            }
            FILE *bin = fopen(binname, "wb");

            // registro de cabecalho inicial
            cabecalho rc = {'0', '0', '1', '2',
                            -1, 0, 0,
                            -1, TAM_CAB2,
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
                    fwrite(&removido, sizeof(char), 1, bin);
                    int prox = -1;
                    fwrite(&prox, sizeof(int), 1, bin);
                    escrever_veiculo(v, bin);

                    // preencher resto do registro com lixo
                    int tamRegistro = sizeof(char) + sizeof(int) + calcular_tamanho(v);
                    char lixo = '$';
                    while (tamRegistro < TAM_TIPO1){
                        fwrite(&lixo, sizeof(char), 1, bin);
                        tamRegistro += sizeof(char);
                    }

                    rc.proxRRN += 1;
                    desalocar_veiculo(v);
                }
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
                    int tamRegistro = sizeof(long) + calcular_tamanho(v);
                    char removido = '0';
                    fwrite(&removido, sizeof(char), 1, bin);
                    fwrite(&tamRegistro, sizeof(int), 1, bin);
                    long int prox = -1;
                    fwrite(&prox, sizeof(long), 1, bin);
                    escrever_veiculo(v, bin);
                    desalocar_veiculo(v);

                    rc.proxByteOffset += sizeof(char) + sizeof(int) + tamRegistro;
                }
            }
            // atualizacao do cabecalho no arquivo binario
            rc.status = '1';
            fseek(bin, 0, SEEK_SET);
            escrever_cabecalho(rc, bin, tipo[4]);

            fclose(csv);
            fclose(bin);
            binarioNaTela(binname);
        }
        break;

        /* funcionalidade 2:
        recuperacao e exibicao de todos os registros armazenados em um arquivo */
        case 2: {
            // leitura do comando e abertura do arquivo
            scanf("%ms %ms", &tipo, &binname);
            FILE *bin = fopen(binname, "rb");
            if (bin == NULL){
                printf("Falha no processamento do arquivo.\n");
                fclose(bin);
                break;
            }

            int qtd = 0;
            // se tipo selecionado for "tipo1"
            if (strcmp(tipo, "tipo1") == 0){
                cabecalho rc = ler_cabecalho(bin, '1');
                if (rc.status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    fclose(bin);
                    break;
                }

                // leitura e exibicao dos registros
                char removido;
                while (fread(&removido, sizeof(char), 1, bin), !feof(bin)){
                    // verificar se registro foi removido
                    if (removido == '1'){
                        fseek(bin, TAM_TIPO1 - sizeof(char), SEEK_CUR);
                        continue;
                    }

                    qtd++;
                    fseek(bin, sizeof(int), SEEK_CUR);
                    veiculo v = ler_veiculo(bin, TAM_TIPO1 - sizeof(char) - sizeof(int));
                    mostrar_veiculo(v);
                    desalocar_veiculo(v);
                }
            }
            // se tipo selecionado for "tipo2"
            else if (strcmp(tipo, "tipo2") == 0){
                cabecalho rc = ler_cabecalho(bin, '2');
                if (rc.status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    fclose(bin);
                    break;
                }

                // leitura e exibicao dos registros
                char removido;
                while (fread(&removido, sizeof(char), 1, bin), !feof(bin)){
                    // verificar se registro foi removido
                    int tamRegistro;
                    fread(&tamRegistro, sizeof(int), 1, bin);
                    if (removido == '1'){
                        fseek(bin, tamRegistro, SEEK_CUR);
                        continue;
                    }

                    qtd++;
                    fseek(bin, sizeof(long), SEEK_CUR);
                    veiculo v = ler_veiculo(bin, tamRegistro - sizeof(long));
                    mostrar_veiculo(v);
                    desalocar_veiculo(v);
                }
            }
            if (qtd == 0) printf("Registro inexistente.\n");
            fclose(bin);
        }
        break;

        /* Funcionalidade 3:
        recuperacao de todos os registros que satisfazem criterios de busca */
        case 3:{
            // leitura do comando
            int nCampos;
            scanf("%ms %ms %d", &tipo, &binname, &nCampos);
            if(strcmp(tipo, "tipo1") && strcmp(tipo, "tipo2")){
                printf("Falha no processamento do arquivo.\n");
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
            if (bin == NULL){
                printf("Falha no processamento do arquivo.\n");
                desalocar_veiculo(f);
                break;
            }
            cabecalho rc = ler_cabecalho(bin, tipo[4]);
            if(rc.status == '0'){
                printf("Falha no processamento do arquivo.\n");
                desalocar_veiculo(f);
                fclose(bin);
                break;
            }

            char c, found = 0;
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
            fclose(bin);
        }
        break;
        
        /* Funcionalidade 4:
        recuperacao e exibicao de registro a partir de RRN */
        case 4:{
            // leitura do comando e abertura do arquivo
            int RRN;
            scanf("%ms %ms %d", &tipo, &binname, &RRN);
            if(strcmp(tipo, "tipo1")){
                printf("Falha no processamento do arquivo.\n");
                break;
            }
            FILE *bin = fopen(binname, "rb");
            if (bin == NULL){
                printf("Falha no processamento do arquivo.\n");
                break;
            }
            cabecalho rc = ler_cabecalho(bin, '1');
            if (rc.status == '0'){
                printf("Falha no processamento do arquivo.\n");
                fclose(bin);
                break;
            }

            // posicionar a leitura do arquivo no registro desejado
            fseek(bin, RRN * TAM_TIPO1, SEEK_CUR); 
            // verificar se registro nao existe ou foi removido
            char removido;
            fread(&removido, sizeof(char), 1, bin);
            if (feof(bin) || (removido == '1')){
                printf("Registro inexistente.\n");
                fclose(bin);
                break;
            }

            // leitura e exibicao do registro
            fseek(bin, sizeof(int), SEEK_CUR);
            veiculo v = ler_veiculo(bin, TAM_TIPO1 - sizeof(char) - sizeof(int));
            mostrar_veiculo(v);

            desalocar_veiculo(v);
            fclose(bin);
        }
        break;

        /* Funcionalidade 5:
        criacao de arquivo de indice primario simples a partir de arquivo de dados */
        case 5:{
            // leitura do comando e abertura dos arquivos
            scanf("%ms %ms %ms", &tipo, &binname, &indname);
            FILE *bin = fopen(binname, "rb");
            FILE *ind = fopen(indname, "wb");
            if (bin == NULL){
                printf("Falha no processamento do arquivo.\n");
                fclose(bin);
                fclose(ind);
                break;
            }
            
            // verificacao do status do arquivo
            cabecalho rc = ler_cabecalho(bin, tipo[4]);
            if (rc.status == '0'){
                printf("Falha no processamento do arquivo.\n");
                fclose(bin);
                fclose(ind);
                break;
            }
            // execucao da funcionalidade
            criar_arquivo_indices(bin, ind, tipo);

            fclose(bin);
            fclose(ind);
            binarioNaTela(indname);
        }
        break;

        /* Funcionalidade 6:
        remocao logica de um registro de um arquivo de dados */
        case 6:{
            // leitura do comando e abertura dos arquivos
            int nRemocoes;
            scanf("%ms %ms %ms %d", &tipo, &binname, &indname, &nRemocoes);
            FILE *bin = fopen(binname, "rb+");
            FILE *ind = fopen(indname, "rb+");
            if (bin == NULL || ind == NULL){
                printf("Falha no processamento do arquivo.\n");
                if (bin != NULL) fclose(bin);
                if (ind != NULL) fclose(ind);
                break;
            }

            // verificacao dos status dos arquivos
            char status;
            fread(&status, sizeof(char), 1, ind);
            cabecalho rc = ler_cabecalho(bin, tipo[4]);
            if (rc.status == '0' || status == '0'){
                printf("Falha no processamento do arquivo.\n");
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

            // leitura dos indices
            int qtd_ind = -1;
            Indice *indices = ler_indices(ind, &qtd_ind, tipo[4]);

            long int inicio; // offset do inicio dos registros de dados
            if (tipo[4] == '1') inicio = TAM_CAB1;
            else if (tipo[4] == '2') inicio = TAM_CAB2;

            while (nRemocoes--){
                // sempre que for realizar uma nova remocao voltar para o inicio dos registros
                fseek(bin, inicio, SEEK_SET);

                // leitura dos criterios de remocao
                int nCampos;
                scanf("%d", &nCampos);
                veiculo f = {-1, -1, -1, "$$", NULL, NULL, NULL};
                while (nCampos--){
                    ler_campo(&f);
                }

                // recupera o offset do primeiro registro que coincide com os criterios
                long int next;
                long int cur = buscar_veiculo(bin, indices, qtd_ind, f, tipo[4], &next);
                if(f.id == -1){
                    // caso a busca nao utilize id continua seguindo pelo arquivo de dados procurando mais registros que
                    // se encaixem nos criterios ate que nao sejam encontrados novos registros
                    while (cur != -1){
                        // guarda onde o ponteiro do arquivo de dados estava para poder seguir para o proximo registro
                        // apos efetuar a remocao
                        long end = ftell(bin);
                        remover_veiculo(bin, cur, tipo[4], &rc);
                        // move o ponteiro do arquivo de dados para o proximo registro
                        fseek(bin, end + next, SEEK_SET);
                        cur = buscar_veiculo(bin, indices, qtd_ind, f, tipo[4], &next);
                    }
                }
                else if(cur != -1){
                    // caso a busca utilize id e tenha sido encontrado um registro, remove apenas esse registro, ja que
                    // ids nao podem ser repetidos
                    remover_veiculo(bin, cur, tipo[4], &rc);
                }
                desalocar_veiculo(f);
                
            }
            free(indices);

            // atualizar arquivo de indices
            if (tipo[4] == '1') fseek(bin, TAM_CAB1, SEEK_SET);
            else if (tipo[4] == '2') fseek(bin, TAM_CAB2, SEEK_SET);
            fclose(ind);
            ind = fopen(indname, "wb");
            criar_arquivo_indices(bin, ind, tipo);

            // atualizar cabecalho do registro de dados
            fseek(bin, 0, SEEK_SET);
            escrever_cabecalho(rc, bin, tipo[4]);

            fclose(bin);
            fclose(ind);
            binarioNaTela(binname);
            binarioNaTela(indname);
        }
        break;

        /* Funcionalidade 7:
        insercao de um novo registro em um arquivo de dados */
        case 7:{
            // leitura do comando e abertura dos arquivos
            int nInsercoes;
            scanf("%ms %ms %ms %d", &tipo, &binname, &indname, &nInsercoes);
            FILE *bin = fopen(binname, "rb+");
            FILE *ind = fopen(indname, "rb+");
            if (bin == NULL || ind == NULL){
                printf("Falha no processamento do arquivo.\n");
                if (bin != NULL) fclose(bin);
                if (ind != NULL) fclose(ind);
                break;
            }

            // verificacao dos status dos arquivos
            char status;
            fread(&status, sizeof(char), 1, ind);
            cabecalho rc = ler_cabecalho(bin, tipo[4]);
            if (rc.status == '0' || status == '0'){
                printf("Falha no processamento do arquivo.\n");
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

            // se tipo selecionado for "tipo1"
            if (strcmp(tipo, "tipo1") == 0){
                // leitura e insercao dos veiculos
                while (nInsercoes--){
                    veiculo v = ler_novo_veiculo(stdin);

                    char removido = '0';
                    int prox = -1;
                    // nao existem registros removidos -> inserir no prox RRN disponivel
                    if (rc.topo1 == -1){
                        fseek(bin, TAM_CAB1 + (rc.proxRRN * TAM_TIPO1), SEEK_SET);
                        fwrite(&removido, sizeof(char), 1, bin);
                        fwrite(&prox, sizeof(int), 1, bin);
                        escrever_veiculo(v, bin);
                        rc.proxRRN++;
                    }
                    // existe registro removido -> inserir no RRN topo da pilha de removidos
                    else{
                        fseek(bin, TAM_CAB1 + (rc.topo1 * TAM_TIPO1), SEEK_SET);
                        fwrite(&removido, sizeof(char), 1, bin);
                        fread(&rc.topo1, sizeof(int), 1, bin); // novo topo: proximo do topo atual
                        fseek(bin, 0 - sizeof(int), SEEK_CUR);
                        fwrite(&prox, sizeof(int), 1, bin);
                        escrever_veiculo(v, bin);
                        rc.nroRegRem--;
                    }

                    // preencher resto do registro com lixo
                    int tamRegistro = sizeof(char) + sizeof(int) + calcular_tamanho(v);
                    char lixo = '$';
                    while (tamRegistro < TAM_TIPO1){
                        fwrite(&lixo, sizeof(char), 1, bin);
                        tamRegistro += sizeof(char);
                    }

                    desalocar_veiculo(v);
                }
                fseek(bin, TAM_CAB1, SEEK_SET); // ponteiro no inicio dos registros
            } 
            // se tipo selecionado for "tipo2"
            else if (strcmp(tipo, "tipo2") == 0){
                // leitura e insercao dos veiculos
                while (nInsercoes--){
                    veiculo v = ler_novo_veiculo(stdin);
                    inserir_veiculo(&rc, bin, v);
                    desalocar_veiculo(v);
                }
                fseek(bin, TAM_CAB2, SEEK_SET); // ponteiro no inicio dos registros
            }

            // atualizar arquivo de indices
            fclose(ind);
            ind = fopen(indname, "wb");
            criar_arquivo_indices(bin, ind, tipo);

            // atualizar cabecalho do registro de dados
            fseek(bin, 0, SEEK_SET);
            escrever_cabecalho(rc, bin, tipo[4]);

            fclose(bin);
            fclose(ind);
            binarioNaTela(binname);
            binarioNaTela(indname);
        }
        break;

        /* Funcionalidade 8:
        atualizacao de registros de um arquivo de dados */
        case 8:{
            // leitura do comando e abertura dos arquivos
            int nAtualizacoes;
            scanf("%ms %ms %ms %d", &tipo, &binname, &indname, &nAtualizacoes);
            FILE *bin = fopen(binname, "rb+");
            FILE *ind = fopen(indname, "rb+");
            if (bin == NULL || ind == NULL){
                printf("Falha no processamento do arquivo.\n");
                if (bin != NULL) fclose(bin);
                if (ind != NULL) fclose(ind);
                break;
            }

            // verificacao dos status dos arquivos
            char status;
            fread(&status, sizeof(char), 1, ind);
            cabecalho rc = ler_cabecalho(bin, tipo[4]);
            if (rc.status == '0' || status == '0'){
                printf("Falha no processamento do arquivo.\n");
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

            // leitura dos indices
            int qtd_ind;
            Indice *indices = ler_indices(ind, &qtd_ind, tipo[4]);

            // se tipo selecionado for "tipo1"
            if (strcmp(tipo, "tipo1") == 0){
                // offset do inicio dos registros de dados
                long inicio = TAM_CAB1;
                while (nAtualizacoes--){
                    // sempre que for realizar uma nova atualizacao voltar para o inicio dos registros
                    fseek(bin, inicio, SEEK_SET);
                    veiculo filtro = {-1, -1, -1, "$$", NULL, NULL, NULL}; // criterios de busca
                    veiculo valores = {-1, -1, -1, "$$", NULL, NULL, NULL}; // valores que devem ser atualizados
                    veiculo campos = {-1, -1, -1, "$$", NULL, NULL, NULL}; // indica os campos que devem ser atualizados

                    // leitura das atualizacoes
                    int nCamposBusca, nCamposAtualiza;
                    scanf("%d ", &nCamposBusca);
                    while (nCamposBusca--){
                        ler_campo(&filtro);
                    }
                    scanf("%d ", &nCamposAtualiza);
                    while (nCamposAtualiza--){
                        ler_novo_campo(&valores, &campos);
                    }

                    // segue lendo os registros ate que nao seja encontrado um proximo registro que coincide com os
                    // criterios de busca desejados
                    long int cur, next;
                    while (cur = buscar_veiculo(bin, indices, qtd_ind, filtro, tipo[4], &next), cur != -1){
                        // pula para o inicio dos dados do veiculo do registro
                        fseek(bin, cur + sizeof(char) + sizeof(int), SEEK_SET);
                        veiculo v = ler_veiculo(bin, TAM_TIPO1);

                        atualizar_veiculo(&v, &valores, &campos);
                        fseek(bin, cur + sizeof(char) + sizeof(int), SEEK_SET);
                        escrever_veiculo(v, bin);

                        // preencher resto do registro com lixo
                        char lixo = '$';
                        int tamRegistro = sizeof(char) + sizeof(int) + calcular_tamanho(v);
                        while (tamRegistro < TAM_TIPO1){
                            fwrite(&lixo, sizeof(char), 1, bin);
                            tamRegistro += sizeof(char);
                        }

                        desalocar_veiculo(v);
                        // caso o filtro utilize um id nao e necessario repetir a tentativa de busca e atualizacao ja
                        // que ids nao podem ser repetidos
                        if(filtro.id != -1) break;
                    }
                    desalocar_veiculo(filtro);
                    desalocar_veiculo(valores);
                    desalocar_veiculo(campos);
                }
                fseek(bin, TAM_CAB1, SEEK_SET); // ponteiro no inicio dos registros
            }
            // se tipo selecionado for "tipo2"
            else if (strcmp(tipo, "tipo2") == 0){
                // offset do inicio dos registros de dados
                long inicio = TAM_CAB2;
                while (nAtualizacoes--){
                    // sempre que for realizar uma nova atualizacao voltar para o inicio dos registros
                    fseek(bin, inicio, SEEK_SET);
                    veiculo filtro = {-1, -1, -1, "$$", NULL, NULL, NULL}; // criterios de busca
                    veiculo valores = {-1, -1, -1, "$$", NULL, NULL, NULL}; // valores que devem ser atualizados
                    veiculo campos = {-1, -1, -1, "$$", NULL, NULL, NULL}; // indica os campos que devem ser atualizados

                    // leitura das atualizacoes
                    int nCamposBusca, nCamposAtualiza;
                    scanf("%d ", &nCamposBusca);
                    while (nCamposBusca--){
                        ler_campo(&filtro);
                    }
                    scanf("%d ", &nCamposAtualiza);
                    while (nCamposAtualiza--){
                        ler_novo_campo(&valores, &campos);
                    }

                    // segue lendo os registros ate que nao seja encontrado um proximo registro que coincide com os
                    // criterios de busca desejados
                    long int cur, next;
                    while (cur = buscar_veiculo(bin, indices, qtd_ind, filtro, tipo[4], &next), cur != -1){
                        // guarda a posicao atual do ponteiro do arquivo de dados para retornar para o proximo registro
                        // apos efetuar a atualizacao
                        long end = ftell(bin);

                        // pula para o campo que guarda o tamanho do registro
                        fseek(bin, cur + sizeof(char), SEEK_SET);

                        int tam;
                        fread(&tam, sizeof(int), 1, bin);
                        // pula para o inicio dos dados do veiculo do registro
                        fseek(bin, sizeof(long), SEEK_CUR);

                        // o tamanho dos dados do veiculo e o tamanho do registro menus o tamanho do campo que guardaria
                        // o offset do proximo registro removido
                        veiculo v = ler_veiculo(bin, tam - sizeof(long));
                        atualizar_veiculo(&v, &valores, &campos);
                        // o tamanho do novo registro sera o tamanho dos dados do veiculo mais o tamanho do campo que
                        // guardaria o offset do proximo registro removido
                        int tamRegistro = calcular_tamanho(v) + sizeof(long);
                        // se dados atualizados cabem no registro existente -> sobrescrever e
                        // completar com lixo (se necessario)
                        if(tamRegistro <= tam){
                            // pula para o inicio dos dados do veiculo do registro
                            fseek(bin, cur + sizeof(char) + sizeof(int) + sizeof(long), SEEK_SET);
                            escrever_veiculo(v, bin);
                            char lixo = '$';
                            while (tamRegistro < tam){
                                fwrite(&lixo, sizeof(char), 1, bin);
                                tamRegistro += sizeof(char);
                            }
                        }
                        // se dados atualizados nao cabem no registro existente -> remover e reinserir
                        else{
                            remover_veiculo(bin, cur, '2', &rc);
                            inserir_veiculo(&rc, bin, v);
                        }

                        desalocar_veiculo(v);
                        // caso o filtro utilize um id nao e necessario repetir a tentativa de busca e atualizacao ja
                        // que ids nao podem ser repetidos
                        if(filtro.id != -1) break;
                        // segue para o inicio do proximo registro
                        fseek(bin, end + next, SEEK_SET);
                    }
                    desalocar_veiculo(filtro);
                    desalocar_veiculo(valores);
                    desalocar_veiculo(campos);
                }
                fseek(bin, TAM_CAB2, SEEK_SET); // ponteiro no inicio dos registros
            }
            free(indices);

            // atualizar arquivo de indices
            fclose(ind);
            ind = fopen(indname, "wb");
            criar_arquivo_indices(bin, ind, tipo);

            // atualizar cabecalho do registro de dados
            fseek(bin, 0, SEEK_SET);
            escrever_cabecalho(rc, bin, tipo[4]);

            fclose(bin);
            fclose(ind);
            binarioNaTela(binname);
            binarioNaTela(indname);
        }
        break;
    }

    if (tipo) free(tipo);
    if (csvname) free(csvname);
    if (binname) free(binname);
    if (indname) free(indname);

    return 0;
}