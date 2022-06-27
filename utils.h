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

void ler_campo(void *f);

void ler_novo_campo(void *valores, void *campos);