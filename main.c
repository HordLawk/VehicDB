#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veiculo.h"
#include "utils.h"
#include "cabecalho.h"

int main(void)
{
    int cmd;
    scanf("%d ", &cmd);
    switch (cmd)
    {
        case 1:
        {
            char *tipo, *csvname, *binname;
            scanf("%ms %ms %ms", &tipo, &csvname, &binname);

            FILE *csv = fopen(csvname, "r");
            FILE *bin = fopen(binname, "wb");
            if (csv == NULL)
            {
                printf("Falha no processamento do arquivo.\n");
                break;
            }

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

            if (strcmp(tipo, "tipo1") == 0)
            {
                escrever_cabecalho(rc, bin, '1');
                linebreak(csv);

                char c;
                while ((c = fgetc(csv)) != EOF)
                {
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
                    while (tamRegistro < 97)
                    {
                        fwrite(&lixo, 1, 1, bin);
                        tamRegistro++;
                    }
                    rc.proxRRN += 1;
                    desalocar_veiculo(v);
                }

                fseek(bin, 174, SEEK_SET);
                fwrite(&rc.proxRRN, 4, 1, bin);
            }
            else if (strcmp(tipo, "tipo2") == 0)
            {
                escrever_cabecalho(rc, bin, '2');
                linebreak(csv);

                char c;
                while ((c = fgetc(csv)) != EOF)
                {
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
                fseek(bin, 178, SEEK_SET);
                fwrite(&rc.proxByteOffset, 8, 1, bin);
            }
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

        case 2:
        {
            // leitura do comando e abertura do arquivo
            char *tipo, *binname;
            scanf("%ms %ms", &tipo, &binname);
            FILE *bin = fopen(binname, "rb");
            if (bin == NULL)
            {
                printf("Falha no processamento do arquivo.\n");
                free(tipo);
                free(binname);
                fclose(bin);
                break;
            }

            int qtd = 0;
            if (strcmp(tipo, "tipo1") == 0)
            {
                cabecalho rc = ler_cabecalho(bin, '1');
                if (rc.status == '0')
                {
                    printf("Falha no processamento do arquivo.\n");
                    free(tipo);
                    free(binname);
                    fclose(bin);
                    break;
                }
                // leitura e exibicao dos registros
                char removido;
                while (fread(&removido, 1, 1, bin), !feof(bin))
                {
                    if (removido == '1')
                    {
                        fseek(bin, 96, SEEK_CUR);
                        continue;
                    }
                    fseek(bin, 4, SEEK_CUR);
                    qtd++;
                    veiculo v = ler_veiculo(bin, 97 - 1 - 4);
                    mostrar_veiculo(v);
                    desalocar_veiculo(v);
                }
            }
            else if (strcmp(tipo, "tipo2") == 0)
            {
                cabecalho rc = ler_cabecalho(bin, '2');
                if (rc.status == '0')
                {
                    printf("Falha no processamento do arquivo.\n");
                    free(tipo);
                    free(binname);
                    fclose(bin);
                    break;
                }

                // leitura e exibicao dos registros
                char removido;
                while (fread(&removido, 1, 1, bin), !feof(bin))
                {
                    int tamRegistro;
                    fread(&tamRegistro, 4, 1, bin);
                    if (removido == '1')
                    {
                        fseek(bin, tamRegistro, SEEK_CUR);
                        continue;
                    }
                    fseek(bin, 8, SEEK_CUR);
                    qtd++;
                    veiculo v = ler_veiculo(bin, tamRegistro - 8);
                    mostrar_veiculo(v);
                    desalocar_veiculo(v);
                }
            }
            if (qtd == 0)
                printf("Registro inexistente.\n");

            free(tipo);
            free(binname);
            fclose(bin);
        }
        break;

        case 3:
        {
            char *tipo, *binname;
            int nCampos;
            scanf("%ms %ms %d", &tipo, &binname, &nCampos);
            veiculo f = {-1, -1, -1, "$$", NULL, NULL, NULL};
            while (nCampos--)
            {
                char *campo;
                scanf("%ms", &campo);
                if (!strcmp(campo, "id"))
                {
                    scanf("%d", &f.id);
                    linebreak(stdin);
                }
                else if (!strcmp(campo, "ano"))
                {
                    scanf("%d", &f.ano);
                    linebreak(stdin);
                }
                else if (!strcmp(campo, "quantidade"))
                {
                    scanf("%d", &f.qtt);
                    linebreak(stdin);
                }
                else if (!strcmp(campo, "sigla"))
                {
                    fgets(f.sigla, 2, stdin);
                    linebreak(stdin);
                }
                else if (!strcmp(campo, "cidade"))
                {
                    char *cidade = calloc(100, sizeof(char));
                    scan_quote_string(cidade);
                    f.cidade = cidade;
                }
                else if (!strcmp(campo, "marca"))
                {
                    char *marca = calloc(100, sizeof(char));
                    scan_quote_string(marca);
                    f.marca = marca;
                }
                else if (!strcmp(campo, "modelo"))
                {
                    char *modelo = calloc(100, sizeof(char));
                    scan_quote_string(modelo);
                    f.modelo = modelo;
                }
                free(campo);
            }
            FILE *bin = fopen(binname, "rb");
            if (bin == NULL)
            {
                printf("Falha no processamento do arquivo.\n");
                desalocar_veiculo(f);
                free(tipo);
                break;
            }

            free(binname);
            cabecalho rc = ler_cabecalho(bin, tipo[4]);
            if(!rc.status){
                printf("Falha no processamento do arquivo.\n");
                desalocar_veiculo(f);
                free(tipo);
                fclose(bin);
                break;
            }
            char c;
            while ((c = fgetc(bin)) != EOF)
            {
                ungetc(c, bin);
                veiculo v = {-1, -1, -1, "$$", NULL, NULL, NULL};
                int next = filtrarVeiculo(bin, f, tipo[4], &v);
                desalocar_veiculo(v);
                if (next) fseek(bin, next, SEEK_CUR);
            }

            desalocar_veiculo(f);
            free(tipo);
            fclose(bin);
        }
        break;
        // OK
        case 4:
        {
            // leitura do comando e abertura do arquivo
            char *tipo, *binname;
            int RRN;
            scanf("%ms %ms %d", &tipo, &binname, &RRN);
            FILE *bin = fopen(binname, "rb");
            if (bin == NULL)
            {
                printf("Falha no processamento do arquivo.\n");
                free(binname);
                free(tipo);
                break;
            }

            cabecalho rc = ler_cabecalho(bin, '1');
            if (rc.status == '0')
            {
                printf("Falha no processamento do arquivo.\n");
                fclose(bin);
                free(binname);
                free(tipo);
                break;
            }
            fseek(bin, RRN * 97, SEEK_CUR);

            // leitura e exibicao do registro
            char removido;
            fread(&removido, 1, 1, bin);
            if (feof(bin) || (removido == '1'))
            {
                printf("Registro inexistente.\n");
                fclose(bin);
                free(binname);
                free(tipo);
                break;
            }
            fseek(bin, 4, SEEK_CUR);
            veiculo v = ler_veiculo(bin, 97 - 1 - 4);
            mostrar_veiculo(v);

            desalocar_veiculo(v);
            fclose(bin);
            free(binname);
            free(tipo);
        }
        break;
    }
    return 0;
}