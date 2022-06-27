#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "veiculo.h"

void linebreak(FILE *stream){
	char crlf = fgetc(stream);
	while ((crlf != '\n') && !feof(stream))
		crlf = fgetc(stream);
}

void binarioNaTela(char *nomeArquivoBinario){
	/* Você não precisa entender o código dessa função. */

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	 *  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))){
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *)malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for (i = 0; i < fl; i++){
		cs += (unsigned long)mb[i];
	}
	printf("%lf\n", (cs / (double)100));
	free(mb);
	fclose(fs);
}

char *scan_quote_string(){
	char *str;
	char R;
	while ((R = getchar()) != EOF && isspace(R));
	if (R == 'N' || R == 'n'){
		getchar();
		getchar();
		getchar();
		str = malloc(sizeof(char));
		strcpy(str, "");
	}
	else if (R == '\"'){
		if (scanf("%m[^\"]", &str) != 1){
			str = malloc(sizeof(char));
			strcpy(str, "");
		}
		getchar();
	}
	else if (R != EOF){
		str = malloc(sizeof(char));
		str[0] = R;
		char *str1 = str + 1;
		scanf("%ms", &str1);
	}
	else{
		str = malloc(sizeof(char));
		strcpy(str, "");
	}
	return str;
}

void ler_campo(void *f){
    char *campo;
    scanf("%ms", &campo);
    if (!strcmp(campo, "id")){
        scanf("%d ", &((veiculo *)f)->id);
    }
    else if (!strcmp(campo, "ano")){
        scanf("%d ", &((veiculo *)f)->ano);
    }
    else if (!strcmp(campo, "quantidade")){
        scanf("%d ", &((veiculo *)f)->qtt);
    }
    else if (!strcmp(campo, "sigla")){
        char *sigla = scan_quote_string();
        strncpy(((veiculo *)f)->sigla, sigla, 2);
        free(sigla);
    }
    else if (!strcmp(campo, "cidade")){
        ((veiculo *)f)->cidade = scan_quote_string();
    }
    else if (!strcmp(campo, "marca")){
        ((veiculo *)f)->marca = scan_quote_string();
    }
    else if (!strcmp(campo, "modelo")){
        ((veiculo *)f)->modelo = scan_quote_string();
    }
    free(campo);
}

void ler_novo_campo(void *valores, void *campos){
    char *campo, *valor;
    scanf("%ms", &campo);
    //printf("campo lido: %s\n", campo);
    if (!strcmp(campo, "id")){
        ((veiculo *)campos)->id = 1;
        scanf("%ms", &valor);
        if (strcmp(valor, "NULO") != 0) ((veiculo *)valores)->id = atoi(valor);
    }
    else if (!strcmp(campo, "ano")){
        ((veiculo *)campos)->ano = 1;
        scanf("%ms", &valor);
        if (strcmp(valor, "NULO") != 0) ((veiculo *)valores)->ano = atoi(valor);
    }
    else if (!strcmp(campo, "qtt")){
        ((veiculo *)campos)->qtt = 1;
        scanf("%ms", &valor);
        if (strcmp(valor, "NULO") != 0) ((veiculo *)valores)->qtt = atoi(valor);
    }
    else if (!strcmp(campo, "sigla")){
        strncpy(((veiculo *)campos)->sigla, "11", 2);
        valor = scan_quote_string();
        if (strcmp(valor, "") != 0) strncpy(((veiculo *)valores)->sigla, valor, 2);
    }
    else if (!strcmp(campo, "cidade")){
        ((veiculo *)campos)->cidade = malloc(2 * sizeof(char));
        strcpy(((veiculo *)campos)->cidade, "1");
        valor = scan_quote_string();
        if (strcmp(valor, "") != 0){
            ((veiculo *)valores)->cidade = realloc(((veiculo *)valores)->cidade, (strlen(valor) * sizeof(char)) + 1);
            strcpy(((veiculo *)valores)->cidade, valor);
        }
    }
    else if (!strcmp(campo, "marca")){
        ((veiculo *)campos)->marca = malloc(2 * sizeof(char));
        strcpy(((veiculo *)campos)->marca, "1");
        valor = scan_quote_string();
        if (strcmp(valor, "") != 0){
            ((veiculo *)valores)->marca = realloc(((veiculo *)valores)->marca, (strlen(valor) * sizeof(char)) + 1);
            strcpy(((veiculo *)valores)->marca, valor);
        }
    }
    else if (!strcmp(campo, "modelo")){
        ((veiculo *)campos)->modelo = malloc(2 * sizeof(char));
        strcpy(((veiculo *)campos)->modelo, "1");
        valor = scan_quote_string();
        if (strcmp(valor, "") != 0){
            ((veiculo *)valores)->modelo = realloc(((veiculo *)valores)->modelo, (strlen(valor) * sizeof(char)) + 1);
            strcpy(((veiculo *)valores)->modelo, valor);
        }
    }
    free(campo);
    free(valor);
}