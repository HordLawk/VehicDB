#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

char *readWord(FILE *stream, char separador)
{
	char *word = NULL;
	char c;
	while ((c = fgetc(stream)) == '\n')
		;
	ungetc(c, stream);
	for (int i = 0; !i || (word[i - 1] != '\0'); i++)
	{
		word = realloc(word, (i + 1) * sizeof(char));
		char tmp = fgetc(stream);
		word[i] = ((tmp == '\n') || feof(stream) || (tmp == '\r') || (tmp == separador)) ? '\0' : tmp;
		if (tmp == '\r')
			fgetc(stream);
	}
	return word;
}

void linebreak(FILE *stream)
{
	char crlf = fgetc(stream);
	while ((crlf != '\n') && !feof(stream))
		crlf = fgetc(stream);
}

void binarioNaTela(char *nomeArquivoBinario)
{ /* Você não precisa entender o código dessa função. */

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	 *  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb")))
	{
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *)malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for (i = 0; i < fl; i++)
	{
		cs += (unsigned long)mb[i];
	}
	printf("%lf\n", (cs / (double)100));
	free(mb);
	fclose(fs);
}

char *scan_quote_string()
{
	char *str;
	char R;
	while ((R = getchar()) != EOF && isspace(R));
	if (R == 'N' || R == 'n')
	{
		getchar();
		getchar();
		getchar();
		str = malloc(sizeof(char));
		strcpy(str, "");
	}
	else if (R == '\"')
	{
		if (scanf("%m[^\"]", &str) != 1)
		{
			str = malloc(sizeof(char));
			strcpy(str, "");
		}
		getchar();
	}
	else if (R != EOF)
	{
		str = malloc(sizeof(char));
		str[0] = R;
		scanf("%ms", &str[1]);
	}
	else
	{
		str = malloc(sizeof(char));
		strcpy(str, "");
	}
	return str;
}
