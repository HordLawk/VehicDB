#include <stdio.h>

#define TAM_TIPO1 97
#define TAM_CAB1 182
#define TAM_CAB2 190

/* funcao disponibilizada na especificacao do trabalho para
verificar se arquivo binario foi escrito corretamente */
void binarioNaTela(char *nomeArquivoBinario);

/* realiza a quebra de linha na leitura de um arquivo;
parametros: ponteiro para arquivo */
void linebreak(FILE *stream);

/* le uma string entre aspas na entrada padrao; retorna string */
char *scan_quote_string();

/* le um campo entrado pelo usuario e registra em uma estrutura veiculo
parametros: estrutura de veiculo para guardar os dados (modificada por referencia) */
void ler_campo(void *f);

/* le um campo que e registra que esse campo deve ser considerado em uma atualizacao
parametros: estrutura veiculo para guardar os valores dos campos (modificada por referencia),
            estrutura veiculo para indicar se um campo deve ser considerado ou nao (modificada por referencia) */
void ler_novo_campo(void *valores, void *campos);