#include <stdio.h>

/* funcao disponibilizada na especificacao do trabalho para
verificar se arquivo binario foi escrito corretamente */
void binarioNaTela(char *nomeArquivoBinario);

/* realiza a quebra de linha na leitura de um arquivo;
parametros: ponteiro para arquivo */
void linebreak(FILE *stream);

/* le uma string entre aspas na entrada padrao; retorna string */
char *scan_quote_string();