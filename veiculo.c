#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veiculo.h"
#include "utils.h"

void mostrar_veiculo(veiculo v){
  printf("MARCA DO VEICULO: %s\n", v.marca == NULL ? "NAO PREENCHIDO" : v.marca);
  printf("MODELO DO VEICULO: %s\n", v.modelo == NULL ? "NAO PREENCHIDO" : v.modelo);
  if (v.ano != -1) printf("ANO DE FABRICACAO: %d\n", v.ano);
  else printf("ANO DE FABRICACAO: NAO PREENCHIDO\n");
  printf("NOME DA CIDADE: %s\n", v.cidade == NULL ? "NAO PREENCHIDO" : v.cidade);
  if (v.qtt != -1) printf("QUANTIDADE DE VEICULOS: %d\n\n", v.qtt);
  else printf("QUANTIDADE DE VEICULOS: NAO PREENCHIDO\n\n");
}

veiculo ler_veiculo(FILE *stream, int tamRegistro){
  veiculo v;
  v.cidade = NULL;
  v.marca = NULL;
  v.modelo = NULL;

  fread(&(v.id), 4, 1, stream);
  fread(&(v.ano), 4, 1, stream);
  fread(&(v.qtt), 4, 1, stream);
  fread(v.sigla, 1, 2, stream);
  int tamAtual = 4 + 4 + 4 + 2;

  char c;
  while ((c = fgetc(stream)) != '$' && tamAtual < tamRegistro){
    ungetc(c, stream);
    
    int tam;
    char codigo;
    fread(&tam, 4, 1, stream);
    fread(&codigo, 1, 1, stream);
    
    if (codigo == '0'){
      v.cidade = malloc((tam + 1) * sizeof(char));
      fread(v.cidade, 1, tam, stream);
      v.cidade[tam] = '\0';
    }
    else if (codigo == '1'){
      v.marca = malloc((tam + 1) * sizeof(char));
      fread(v.marca, 1, tam, stream);
      v.marca[tam] = '\0';
    }
    else if (codigo == '2'){
      v.modelo = malloc((tam + 1) * sizeof(char));
      fread(v.modelo, 1, tam, stream);
      v.modelo[tam] = '\0';
    }
    tamAtual += 4 + 1 + tam;
  }
  ungetc(c, stream);

  if (tamAtual < tamRegistro)
    fseek(stream, tamRegistro-tamAtual, SEEK_CUR);
    
  return v;
}

void escrever_veiculo(veiculo v, FILE *stream){
  fwrite(&v.id, 4, 1, stream);
  fwrite(&v.ano, 4, 1, stream);
  fwrite(&v.qtt, 4, 1, stream);
  fwrite(v.sigla, 1, 2, stream);

  int tamCidade = strlen(v.cidade), tamMarca = strlen(v.marca), tamModelo = strlen(v.modelo);
  if (tamCidade > 0){
      char codigo = '0';
      fwrite(&tamCidade, 4, 1, stream);
      fwrite(&codigo, 1, 1, stream);
      fwrite(v.cidade, 1, tamCidade, stream);
  }
  if (tamMarca > 0){
      char codigo = '1';
      fwrite(&tamMarca, 4, 1, stream);
      fwrite(&codigo, 1, 1, stream);
      fwrite(v.marca, 1, tamMarca, stream);
  }
  if (tamModelo > 0){
      char codigo = '2';
      fwrite(&tamModelo, 4, 1, stream);
      fwrite(&codigo, 1, 1, stream);
      fwrite(v.modelo, 1, tamModelo, stream);
  }
}

int calcular_tamanho(veiculo v){
  int tamanho = 4 + 4 + 4 + 2;
  if (strlen(v.cidade) > 0)
      tamanho += 1 + 4 + strlen(v.cidade);
  if (strlen(v.marca) > 0)
      tamanho += 1 + 4 + strlen(v.marca);
  if (strlen(v.modelo) > 0)
      tamanho += 1 + 4 + strlen(v.modelo);
  return tamanho;
}

void desalocar_veiculo(veiculo v){
  if (v.cidade) free(v.cidade);
  if (v.marca) free(v.marca);
  if (v.modelo) free(v.modelo);
}

veiculo ler_veiculo_csv(FILE *stream){
  veiculo v;
  char *id = readWord(stream, ',');
  v.id = atoi(id);
  char *ano = readWord(stream, ',');
  v.ano = strlen(ano) == 0 ? -1 : atoi(ano);
  v.cidade = readWord(stream, ',');
  char *qtt = readWord(stream, ',');
  v.qtt = strlen(qtt) == 0 ? -1 : atoi(qtt);
  char *sigla = readWord(stream, ',');
  if (strlen(sigla) == 0) 
    strcpy(v.sigla, "$$");
  else 
    strcpy(v.sigla, sigla);
  v.marca = readWord(stream, ',');
  v.modelo = readWord(stream, ',');

  free(id);
  free(qtt);
  free(sigla);
  
  return v;
}