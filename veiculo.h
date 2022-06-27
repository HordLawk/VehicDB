#include <stdio.h>

/* struct com os dados de um veiculo */
typedef struct Veiculo{
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

/* verifica se um veiculo corresponde a um ou mais criterios de busca
retorna offset para o inicio do registro checado se o mesmo coincidir com os criterios de busca ou -1 caso contrario
parametros: ponteiro para arquivo, veiculo com criterios de busca, tipo do arquivo,
            veiculo a ser lido (passado por referencia),
            distancia entre o ponteiro do arquivo e o inicio do proximo registro (modificado por referencia) */
long int filtrarVeiculo(FILE *stream, veiculo f, char tipo, veiculo *v, long int *next);

/* le um novo veiculo como input
retorna um objeto do tipo veiculo com os dados entrados
parametros: fluxo de entrada */
veiculo ler_novo_veiculo(FILE *stream);

/* busca um veiculo a partir de um criterio de busca e usa uma lista de indices caso incluirem o id
retorna o offset para o primeiro registro que coincide com os criterios
parametros: ponteiro para arquivo de dados, lista de indices referente ao arquivo de dados, quantidade de indices,
            objeto veiculo contendo os criterios de busca, tipo do arquivo de dados,
            distancia entre o ponteiro do arquivo e o inicio do proximo registro (modificado por referencia) */
long int buscar_veiculo(FILE *stream, void *indices, int qtd_ind, veiculo f, char tipo, long int *next);

/* atualiza uma estrutura do tipo veiculo com novos dados entrados
parametros: o veiculo para atualizar, os valores para serem definidos,
            uma estrutura veiculo indicando quais campos devem ser atualizados */
void atualizar_veiculo(veiculo *v, veiculo *valores, veiculo *campos);

/* marca um registro como logicamente removido
parametros: ponteiro para o arquivo de dados, o offset do registro para ser removido, o tipo do arquivo de dados,
            ponteiro para uma estrutura contendo os dados do cabecalho do arquivo de dados (passado por referencia) */
void remover_veiculo(FILE *bin, long cur, char tipo, void *rc);

/* insere um novo registro no arquivo de dados utilizando o espaco de arquivos logicamente removidos se possivel
parametros: ponteiro para uma estrutura contendo os dados do cabecalho do arquivo de dados (passado por referencia),
            ponteiro para o arquivo de dados, o veiculo a ser inserido */
void inserir_veiculo(void *rc, FILE *bin, veiculo v);