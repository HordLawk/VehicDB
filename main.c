#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veiculo.h"
#include "utils.h"

int filtrar_veiculo1(FILE *stream, veiculo f);
int filtrar_veiculo2(FILE *stream, veiculo f);

typedef struct cabecalho{
  char status, codC5, codC6, codC7;
  int topo1, proxRRN, nroRegRem;
  long topo2, proxByteOffset;
  char descricao[40], desC1[22], desC2[19], desC3[24], desC4[8], desC5[16], desC6[18], desC7[19];
  
} cabecalho;

void escrever_cabecalho(cabecalho c, FILE *stream, char tipo){
  fwrite(&c.status, 1, 1, stream);
  fwrite(c.descricao, 1, 40, stream);

  if (tipo == '1') fwrite(&c.topo1, 4, 1, stream);
  else fwrite(&c.topo2, 8, 1, stream);

  fwrite(c.desC1, 1, 22, stream);
  fwrite(c.desC2, 1, 19, stream);
  fwrite(c.desC3, 1, 24, stream);
  fwrite(c.desC4, 1, 8, stream);
  fwrite(&c.codC5, 1, 1, stream);
  fwrite(c.desC5, 1, 16, stream);
  fwrite(&c.codC6, 1, 1, stream);
  fwrite(c.desC6, 1, 18, stream);
  fwrite(&c.codC7, 1, 1, stream);
  fwrite(c.desC7, 1, 19, stream);

  if (tipo == '1') fwrite(&c.proxRRN, 4, 1, stream);
  else fwrite(&c.proxByteOffset, 8, 1, stream);
  fwrite(&c.nroRegRem, 4, 1, stream);
}

cabecalho ler_cabecalho(FILE *stream, char tipo){
  // pq eu faria qualquer uma dessas coisas
  cabecalho c;
  fread(&c.status, 1, 1, stream);
  fread(&c.descricao, 1, 40, stream);

  if (tipo == '1') fread(&c.topo1, 4, 1, stream);
  else fread(&c.topo2, 8, 1, stream);
    
  fread(c.desC1, 1, 22, stream);
  fread(c.desC2, 1, 19, stream);
  fread(c.desC3, 1, 24, stream);
  fread(c.desC4, 1, 8, stream);
  fread(&c.codC5, 1, 1, stream);
  fread(c.desC5, 1, 16, stream);
  fread(&c.codC6, 1, 1, stream);
  fread(c.desC6, 1, 18, stream);
  fread(&c.codC7, 1, 1, stream);
  fread(c.desC7, 1, 19, stream);

  if (tipo == '1') fread(&c.proxRRN, 4, 1, stream);
  else fread(&c.proxByteOffset, 8, 1, stream);

  fread(&c.nroRegRem, 4, 1, stream);

  return c;
}

int main(void) {
    int cmd;
    scanf("%d ", &cmd);
    switch(cmd){
        case 1: {
            char *tipo, *csvname, *binname;
            scanf("%ms %ms %ms", &tipo, &csvname, &binname);
            
            FILE* csv = fopen(csvname, "r");
            FILE* bin = fopen(binname, "wb");
            if (csv == NULL){
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
                "MODELO DO VEICULO: "
            };

            if(strcmp(tipo, "tipo1") == 0){
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
              
                fseek(bin, 174, SEEK_SET);
                fwrite(&rc.proxRRN, 4, 1, bin);
            }
            else if(strcmp(tipo, "tipo2") == 0){
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
                  
                    rc.proxByteOffset += 1 + 4 + tamRegistro;
                }
                // atualizar status
                // reescrever cabecalho
            }
            // escrever status??
          
            fclose(csv);
            fclose(bin);

            binarioNaTela(binname);
            free(binname);
            free(csvname);
            free(tipo);
        }
        break;
      
        case 2: {
          // leitura do comando e abertura do arquivo
          char *tipo, *binname;
          scanf("%ms %ms", &tipo, &binname);
          FILE* bin = fopen(binname, "rb");
          if (bin == NULL){
            printf("Falha no processamento do arquivo.\n");
            break;
          }

          if (strcmp(tipo, "tipo1") == 0){
            fseek(bin, 182, SEEK_SET); // talvez ler cabecalho etc
            // checar se status e 0

            // leitura e exibicao dos registros
            char removido;
            while (fread(&removido, 1, 1, bin), !feof(bin)){
              if (removido == '1'){
                fseek(bin, 96, SEEK_CUR);
                continue;
              }  
              fseek(bin, 4, SEEK_CUR);
              veiculo v = ler_veiculo(bin, 97-1-4);
              mostrar_veiculo(v);
              desalocar_veiculo(v);
            } 
          }
          else if (strcmp(tipo, "tipo2") == 0){
            fseek(bin, 190, SEEK_SET); // talvez ler cabecalho etc

            // leitura e exibicao dos registros
            char removido;
            while (fread(&removido, 1, 1, bin), !feof(bin)){
              int tamRegistro;
              fread(&tamRegistro, 4, 1, bin);
              if (removido == '1'){
                fseek(bin, tamRegistro, SEEK_CUR);
                continue;
              }
              fseek(bin, 8, SEEK_CUR);
              veiculo v = ler_veiculo(bin, tamRegistro-8);
              mostrar_veiculo(v);
              desalocar_veiculo(v);
            } 
          }
          free(tipo);
          free(binname);
          fclose(bin);
        }
        break;

        case 3: {
            char* tipo = readWord(stdin, ' ');
            char* binname = readWord(stdin, ' ');
            int nCampos;
            scanf("%d", &nCampos);
            linebreak(stdin);
            veiculo f = {-1, -1, -1, "$$", NULL, NULL, NULL};
            while(nCampos--){
                char* campo = readWord(stdin, ' ');
                if(!strcmp(campo, "id")){
                    scanf("%d", &f.id);
                    linebreak(stdin);
                }
                else if(!strcmp(campo, "ano")){
                    scanf("%d", &f.ano);
                    linebreak(stdin);
                }
                else if(!strcmp(campo, "quantidade")){
                    scanf("%d", &f.qtt);
                    linebreak(stdin);
                }
                else if(!strcmp(campo, "sigla")){
                    fgets(f.sigla, 2, stdin);
                    linebreak(stdin);
                }
                else if(!strcmp(campo, "cidade")){
                    char cidade[100];
                    scan_quote_string(cidade);
                    f.cidade = cidade;
                }
                else if(!strcmp(campo, "marca")){
                    char* marca = malloc(100 * sizeof(char));
                    scan_quote_string(marca);
                    f.marca = marca;
                }
                else if(!strcmp(campo, "modelo")){
                    char modelo[100];
                    scan_quote_string(modelo);
                    f.modelo = modelo;
                }
                free(campo);
            }
            FILE* bin = fopen(binname, "rb");
            if (bin == NULL){
              printf("Falha no processamento do arquivo.\n");
              break;
            }
          // checar se status e 0


          
            free(binname);
            if (strcmp(tipo, "tipo1") == 0){
                char c;
                fseek(bin, 182, SEEK_SET); 
                while ((c = fgetc(bin)) != EOF){
                    ungetc(c, bin);
                    int next = 97 - filtrar_veiculo1(bin, f);
                    if(next) fseek(bin, next, SEEK_CUR);
                } 
            }
            else if (strcmp(tipo, "tipo2") == 0){
                char c;
                fseek(bin, 190, SEEK_SET);
                while ((c = fgetc(bin)) != EOF){
                    ungetc(c, bin);
                    int next = filtrar_veiculo2(bin, f);
                    if(next) fseek(bin, next, SEEK_CUR);
                } 
            }
            
            free(tipo);
            fclose(bin);
        }
        break;
        // OK
        case 4: {
          // leitura do comando e abertura do arquivo
          char *tipo, *binname;
          int RRN;
          scanf("%ms %ms %d", &tipo, &binname, &RRN);
          FILE* bin = fopen(binname, "rb");
          if (bin == NULL){
            printf("Falha no processamento do arquivo.\n");
            break;
          }
          
          // talvez ler cabecalho? ainda n sei como isso funciona
          // checar se status e 0
          fseek(bin, 182 + RRN * 97, SEEK_SET);

          // leitura e exibicao do registro
          char removido;
          fread(&removido, 1, 1, bin);
          if (removido == '1' || feof(bin)){
            printf("Registro inexistente.\n");
            break;
          }
          fseek(bin, 4, SEEK_CUR);
          veiculo v = ler_veiculo(bin, 97-1-4);
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

int filtrar_veiculo1(FILE *stream, veiculo f){
    char removido;
    fread(&removido, 1, 1, stream);
    if(removido == '1') return 1;
    fseek(stream, 4, SEEK_CUR);
    int id;
    fread(&id, 4, 1, stream);
    if((f.id != -1) && (id != f.id)) return 9;
    int ano;
    fread(&ano, 4, 1, stream);
    if((f.ano != -1) && (ano != f.ano)) return 13;
    int qtt;
    fread(&qtt, 4, 1, stream);
    if((f.qtt != -1) && (qtt != f.qtt)) return 17;
    char sigla[2];
    fread(sigla, 1, 2, stream);
    if(strcmp(f.sigla, "$$") && strcmp(sigla, f.sigla)) return 19;
    int lido = 19;
    
    char cidade[100] = "";
    char modelo[100] = "";
    char marca[100] = "";
    
    char c = fgetc(stream);
    ungetc(c, stream);
    while ((lido < 97) && (c != '$')){
        
        int tam;
        char codigo;
        fread(&tam, 4, 1, stream);
        fread(&codigo, 1, 1, stream);
        lido += 5 + tam;
        
        if (codigo == '0'){
            fread(cidade, 1, tam, stream);
            cidade[tam] = '\0';
            if(f.cidade && strcmp(cidade, f.cidade)) return lido;
        }
        else if (codigo == '1'){
            fread(marca, 1, tam, stream);
            marca[tam] = '\0';
            if(f.marca && strcmp(marca, f.marca)) return lido;
        }
        else if (codigo == '2'){
            fread(modelo, 1, tam, stream);
            modelo[tam] = '\0';
            if(f.modelo && strcmp(modelo, f.modelo)) return lido;
        }

        c = fgetc(stream);
        ungetc(c, stream);
    }
    if((f.cidade && !strlen(cidade)) || (f.modelo && !strlen(modelo)) || (f.marca && !strlen(marca))) return lido;
    
    printf("marca: %s\n", strlen(marca) == 0 ? "NAO PREENCHIDO" : marca);
    printf("modelo: %s\n", strlen(modelo) == 0 ? "NAO PREENCHIDO" : modelo);
    printf("ano: %d\n", ano);
    printf("cidade: %s\n", strlen(cidade) == 0 ? "NAO PREENCHIDO" : cidade);
    printf("qtt: %d\n\n", qtt);

    return lido;
}

int filtrar_veiculo2(FILE *stream, veiculo f){
    char removido;
    fread(&removido, 1, 1, stream);
    int tamRegistro;
    fread(&tamRegistro, 4, 1, stream);
    if(removido == '1') return tamRegistro;
    fseek(stream, 8, SEEK_CUR);
    int id;
    fread(&id, 4, 1, stream);
    if((f.id != -1) && (id != f.id)) return tamRegistro - 12;
    int ano;
    fread(&ano, 4, 1, stream);
    if((f.ano != -1) && (ano != f.ano)) return tamRegistro - 16;
    int qtt;
    fread(&qtt, 4, 1, stream);
    if((f.qtt != -1) && (qtt != f.qtt)) return tamRegistro - 20;
    char sigla[2];
    fread(sigla, 1, 2, stream);
    if(strcmp(f.sigla, "$$") && strcmp(sigla, f.sigla)) return tamRegistro - 22;
    int tamAtual = 8 + 4 + 4 + 4 + 2;
    
    char cidade[100] = "";
    char modelo[100] = "";
    char marca[100] = "";
    
    while (tamAtual < tamRegistro){
        int tam;
        char codigo;
        fread(&tam, 4, 1, stream);
        fread(&codigo, 1, 1, stream);
        tamAtual += 4 + 1 + tam;
        
        if (codigo == '0'){
            fread(cidade, 1, tam, stream);
            cidade[tam] = '\0';
            if(f.cidade && strcmp(cidade, f.cidade)) return tamRegistro - tamAtual;
        }
        else if (codigo == '1'){
            fread(marca, 1, tam, stream);
            marca[tam] = '\0';
            if(f.marca && strcmp(marca, f.marca)) return tamRegistro - tamAtual;
        }
        else if (codigo == '2'){
            fread(modelo, 1, tam, stream);
            modelo[tam] = '\0';
            if(f.modelo && strcmp(modelo, f.modelo)) return tamRegistro - tamAtual;
        }
    }
    if((f.cidade && !strlen(cidade)) || (f.modelo && !strlen(modelo)) || (f.marca && !strlen(marca))) return tamRegistro - tamAtual;
    
    printf("marca: %s\n", strlen(marca) == 0 ? "NAO PREENCHIDO" : marca);
    printf("modelo: %s\n", strlen(modelo) == 0 ? "NAO PREENCHIDO" : modelo);
    printf("ano: %d\n", ano);
    printf("cidade: %s\n", strlen(cidade) == 0 ? "NAO PREENCHIDO" : cidade);
    printf("qtt: %d\n\n", qtt);

    return tamRegistro - tamAtual;
}