#include <stdio.h>

/* struct com os dados de um veiculo */
typedef struct Veiculo
{
    int id;
    int ano;
    int qtt;
    char sigla[2];
    char *cidade;
    char *marca;
    char *modelo;
} veiculo;

/* le os dados de um veiculo em um arquivo binario; retorna veiculo lido
parametros: ponteiro para arquivo, tamanho do registro */
veiculo ler_veiculo(FILE *stream, int tamRegistro);

/* exibe os dados de um veiculo com a formatacao especificada
parametros: veiculo */
void mostrar_veiculo(veiculo v);

/* escreve os dados de um veiculo em um arquivo binario
parametros: veiculo, ponteiro para arquivo */
void escrever_veiculo(veiculo v, FILE *stream);

/* calcula o tamanho que os dados de um veiculo ocupam no formato do arquivo binario; retorna tamanho
parametros: veiculo*/
int calcular_tamanho(veiculo v);

/* desaloca a memoria das strings alocadas dinamicamente em um veiculo
parametros: veiculo */
void desalocar_veiculo(veiculo v);

/* le os dados de um veiculo em um arquivo csv; retorna veiculo lido
parametros: ponteiro para arquivo */
veiculo ler_veiculo_csv(FILE *stream);

/* verifica se um veiculo corresponde a um ou mais criterios de busca, se sim exibe seus dados; 
retorna offset para o proximo registro do arquivo binario    
parametros: ponteiro para arquivo, veiculo com criterios de busca, tipo do arquivo, 
            veiculo a ser lido (passado por referencia) */
int filtrarVeiculo(FILE *stream, veiculo f, char tipo, veiculo *v);