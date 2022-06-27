#include <stdio.h>
#include <stdlib.h>
#include "cabecalho.h"

void escrever_cabecalho(cabecalho c, FILE *stream, char tipo){
    fwrite(&c.status, sizeof(char), 1, stream);

    if (tipo == '1')
        fwrite(&c.topo1, sizeof(int), 1, stream);
    else
        fwrite(&c.topo2, sizeof(long), 1, stream);

    fwrite(c.descricao, sizeof(char), 40, stream);
    fwrite(c.desC1, sizeof(char), 22, stream);
    fwrite(c.desC2, sizeof(char), 19, stream);
    fwrite(c.desC3, sizeof(char), 24, stream);
    fwrite(c.desC4, sizeof(char), 8, stream);
    fwrite(&c.codC5, sizeof(char), 1, stream);
    fwrite(c.desC5, sizeof(char), 16, stream);
    fwrite(&c.codC6, sizeof(char), 1, stream);
    fwrite(c.desC6, sizeof(char), 18, stream);
    fwrite(&c.codC7, sizeof(char), 1, stream);
    fwrite(c.desC7, sizeof(char), 19, stream);

    if (tipo == '1')
        fwrite(&c.proxRRN, sizeof(int), 1, stream);
    else
        fwrite(&c.proxByteOffset, sizeof(long), 1, stream);
    fwrite(&c.nroRegRem, sizeof(int), 1, stream);
}

cabecalho ler_cabecalho(FILE *stream, char tipo){
    cabecalho c;
    fread(&c.status, sizeof(char), 1, stream);

    if (tipo == '1')
        fread(&c.topo1, sizeof(int), 1, stream);
    else
        fread(&c.topo2, sizeof(long), 1, stream);

    fread(&c.descricao, sizeof(char), 40, stream);
    fread(c.desC1, sizeof(char), 22, stream);
    fread(c.desC2, sizeof(char), 19, stream);
    fread(c.desC3, sizeof(char), 24, stream);
    fread(c.desC4, sizeof(char), 8, stream);
    fread(&c.codC5, sizeof(char), 1, stream);
    fread(c.desC5, sizeof(char), 16, stream);
    fread(&c.codC6, sizeof(char), 1, stream);
    fread(c.desC6, sizeof(char), 18, stream);
    fread(&c.codC7, sizeof(char), 1, stream);
    fread(c.desC7, sizeof(char), 19, stream);

    if (tipo == '1')
        fread(&c.proxRRN, sizeof(int), 1, stream);
    else
        fread(&c.proxByteOffset, sizeof(long), 1, stream);
    fread(&c.nroRegRem, sizeof(int), 1, stream);

    return c;
}