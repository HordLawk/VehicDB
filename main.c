#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veiculo.h"
#include "utils.h"
#include "cabecalho.h"
#include "indice.h"

#define TAM_TIPO1 97

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
                // atualizacao do cabecalho no arquivo binario
                fseek(bin, 174, SEEK_SET);
                fwrite(&rc.proxRRN, sizeof(int), 1, bin);
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
                // atualizacao do cabecalho no arquivo binario
                fseek(bin, 178, SEEK_SET);
                fwrite(&rc.proxByteOffset, sizeof(long), 1, bin);
            }
            // atualizacao do cabecalho no arquivo binario
            rc.status = '1';
            fseek(bin, 0, SEEK_SET);
            fwrite(&rc.status, sizeof(char), 1, bin);

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

            // verificacao dos status e leitura dos indices
            int qtd_ind = -1;
            Indice *indices = NULL;
            cabecalho rc = ler_cabecalho(bin, tipo[4]);
            if (strcmp(tipo, "tipo1") == 0){
                if (rc.status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    fclose(bin);
                    fclose(ind);
                    break;
                }
                indices = ler_indices(ind, &qtd_ind, '1');
            } 
            else if (strcmp(tipo, "tipo2") == 0){
                if (rc.status == '0'){
                    printf("Falha no processamento do arquivo.\n");
                    fclose(bin);
                    fclose(ind);
                    break;
                }
                indices = ler_indices(ind, &qtd_ind, '2');
            }

            // remocoes - por favor comentar
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
                if(f.id == -1){
                    while (cur != -1){
                        long end = ftell(bin);
                        remover_veiculo(bin, cur, tipo[4], &rc);
                        fseek(bin, end + next, SEEK_SET);
                        cur = buscar_veiculo(bin, indices, qtd_ind, f, tipo[4], &next);
                    }
                }
                else if(cur != -1){
                    remover_veiculo(bin, cur, tipo[4], &rc);
                }
                
            }
            fseek(bin, 0, SEEK_SET);
            escrever_cabecalho(rc, bin, tipo[4]);
            fclose(ind);
            ind = fopen(indname, "wb");
            criar_arquivo_indices(bin, ind, tipo);
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

            char status;
            fread(&status, sizeof(char), 1, ind);
            // se tipo selecionado for "tipo1"
            if (strcmp(tipo, "tipo1") == 0){
                // verificacao dos status dos arquivos
                cabecalho rc = ler_cabecalho(bin, '1');
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

                // leitura e insercao dos veiculos
                while (nInsercoes--){
                    veiculo v = ler_novo_veiculo(stdin);

                    char removido = '0';
                    int prox = -1;
                    // nao existem registros removidos -> inserir no prox RRN disponivel
                    if (rc.topo1 == -1){
                        fseek(bin, 182 + (rc.proxRRN * TAM_TIPO1), SEEK_SET);
                        fwrite(&removido, sizeof(char), 1, bin);
                        fwrite(&prox, sizeof(int), 1, bin);
                        escrever_veiculo(v, bin);
                        rc.proxRRN++;
                    }
                    // existe registro removido -> inserir no RRN topo da pilha de removidos
                    else{
                        fseek(bin, 182 + (rc.topo1 * TAM_TIPO1), SEEK_SET);
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

                // atualizar arquivo de indices
                fseek(bin, 182, SEEK_SET);
                fclose(ind);
                ind = fopen(indname, "wb");
                criar_arquivo_indices(bin, ind, tipo);

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
                    inserir_veiculo(&rc, bin, v);
                    desalocar_veiculo(v);
                }

                // atualizar arquivo de indices
                fseek(bin, 190, SEEK_SET);
                fclose(ind);
                ind = fopen(indname, "wb");
                criar_arquivo_indices(bin, ind, tipo);

                // atualizar cabecalho do registro de dados
                fseek(bin, 0, SEEK_SET);
                escrever_cabecalho(rc, bin, '2');
            }

            fclose(bin);
            fclose(ind);
            binarioNaTela(binname);
            binarioNaTela(indname);
        }
        break;

        /* Funcionalidade 8:
        atualizacao de registros de um arquivo de dados */
        case 8:{
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

            char status;
            fread(&status, sizeof(char), 1, ind);

            // se tipo selecionado for "tipo1"
            if (strcmp(tipo, "tipo1") == 0){
                // verificacao dos status dos arquivos
                cabecalho rc = ler_cabecalho(bin, '1');
                if (rc.status == '0' || status == '0'){
                    printf("Falha no processamento do arquivo.\n");
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

                    long int cur, next;
                    while (cur = buscar_veiculo(bin, indices, qtd_ind, filtro, tipo[4], &next), cur != -1){
                        fseek(bin, cur + sizeof(char) + sizeof(int), SEEK_SET);
                        veiculo v = ler_veiculo(bin, TAM_TIPO1);

                        atualizar_veiculo_1(&v, &valores, &campos);
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
                criar_arquivo_indices(bin, ind, tipo);

                fclose(bin);
                fclose(ind);
                binarioNaTela(binname);
                binarioNaTela(indname);
            }
            // se tipo selecionado for "tipo2"
            else if (strcmp(tipo, "tipo2") == 0){
                // verificacao dos status dos arquivos
                cabecalho rc = ler_cabecalho(bin, '2');
                if (rc.status == '0' || status == '0'){
                    printf("Falha no processamento do arquivo.\n");
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

                    Indice *indices = ler_indices(ind, &qtd_ind, '2');
                    long int cur, next;
                    while (cur = buscar_veiculo(bin, indices, qtd_ind, filtro, tipo[4], &next), cur != -1){
                        long end = ftell(bin);

                        fseek(bin, cur + sizeof(char), SEEK_SET);

                        int tam;
                        fread(&tam, sizeof(int), 1, bin);
                        fseek(bin, sizeof(long), SEEK_CUR);

                        veiculo v = ler_veiculo(bin, tam - sizeof(long));
                        atualizar_veiculo_1(&v, &valores, &campos);
                        int tamreg = calcular_tamanho(v) + sizeof(long);
                        if(tamreg <= tam){
                            fseek(bin, cur + sizeof(char) + sizeof(int) + sizeof(long), SEEK_SET);
                            escrever_veiculo(v, bin);
                            char lixo = '$';
                            while (tamreg < tam){
                                fwrite(&lixo, sizeof(char), 1, bin);
                                tamreg += sizeof(char);
                            }
                        }
                        else{
                            remover_veiculo(bin, cur, '2', &rc);
                            inserir_veiculo(&rc, bin, v);
                        }

                        desalocar_veiculo(v);
                        if(filtro.id != -1) break;
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
                criar_arquivo_indices(bin, ind, tipo);

                // atualizar cabecalho do registro de dados
                fseek(bin, 0, SEEK_SET);
                escrever_cabecalho(rc, bin, '2');

                fclose(bin);
                fclose(ind);
                binarioNaTela(binname);
                binarioNaTela(indname);
            }
        }
        break;
    }

    if (tipo) free(tipo);
    if (csvname) free(csvname);
    if (binname) free(binname);
    if (indname) free(indname);

    return 0;
}