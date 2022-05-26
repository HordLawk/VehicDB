#include <stdio.h>

typedef struct Veiculo{
    int id;
    int ano;
    int qtt;
    char sigla[3];
    char *cidade;
    char *marca;
    char *modelo;
} veiculo;

veiculo ler_veiculo(FILE *stream, int tamRegistro);
void mostrar_veiculo(veiculo v);
void escrever_veiculo(veiculo v, FILE *stream);
int calcular_tamanho(veiculo v);
void desalocar_veiculo(veiculo v);
veiculo ler_veiculo_csv(FILE *stream);