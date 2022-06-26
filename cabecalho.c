#include <stdio.h>
#include <stdlib.h>
#include "cabecalho.h"

void escrever_cabecalho(cabecalho c, FILE *stream, char tipo){
    fwrite(&c.status, 1, 1, stream);

    if (tipo == '1')
        fwrite(&c.topo1, 4, 1, stream);
    else
        fwrite(&c.topo2, 8, 1, stream);

    fwrite(c.descricao, 1, 40, stream);
    fwrite(c.desC1, 1, 22, stream);
    fwrite(c.desC2, 1, 19, stream);
    fwrite(c.desC3, 1, 24, stream);
    fwrite(c.desC4, 1, 8, stream);
    fwrite(&c.codC5, 1, 1, stream);
    fwrite(c.desC5, 1, 16, stream);
    fwrite(&c.codC6, 1, 1, stream);
    fwrite(c.desC6, 1, 18, stream);
    fwrite(&c.codC7, 1, 1, stream);
    fwrite(c.desC7, 1, 19, stream);

    if (tipo == '1')
        fwrite(&c.proxRRN, 4, 1, stream);
    else
        fwrite(&c.proxByteOffset, 8, 1, stream);
    fwrite(&c.nroRegRem, 4, 1, stream);
}

cabecalho ler_cabecalho(FILE *stream, char tipo){
    cabecalho c;
    fread(&c.status, 1, 1, stream);

    if (tipo == '1')
        fread(&c.topo1, 4, 1, stream);
    else
        fread(&c.topo2, 8, 1, stream);

    fread(&c.descricao, 1, 40, stream);

    fread(c.desC1, 1, 22, stream);
    fread(c.desC2, 1, 19, stream);
    fread(c.desC3, 1, 24, stream);
    fread(c.desC4, 1, 8, stream);
    fread(&c.codC5, 1, 1, stream);
    fread(c.desC5, 1, 16, stream);
    fread(&c.codC6, 1, 1, stream);
    fread(c.desC6, 1, 18, stream);
    fread(&c.codC7, 1, 1, stream);
    fread(c.desC7, 1, 19, stream);

    if (tipo == '1')
        fread(&c.proxRRN, 4, 1, stream);
    else
        fread(&c.proxByteOffset, 8, 1, stream);

    fread(&c.nroRegRem, 4, 1, stream);

    return c;
}