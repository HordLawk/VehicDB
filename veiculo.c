#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veiculo.h"
#include "indice.h"
#include "utils.h"
#include "cabecalho.h"

void mostrar_veiculo(veiculo v){
    printf("MARCA DO VEICULO: %s\n", v.marca == NULL ? "NAO PREENCHIDO" : v.marca);
    printf("MODELO DO VEICULO: %s\n", v.modelo == NULL ? "NAO PREENCHIDO" : v.modelo);
    if (v.ano != -1)
        printf("ANO DE FABRICACAO: %d\n", v.ano);
    else
        printf("ANO DE FABRICACAO: NAO PREENCHIDO\n");
    printf("NOME DA CIDADE: %s\n", v.cidade == NULL ? "NAO PREENCHIDO" : v.cidade);
    if (v.qtt != -1)
        printf("QUANTIDADE DE VEICULOS: %d\n\n", v.qtt);
    else
        printf("QUANTIDADE DE VEICULOS: NAO PREENCHIDO\n\n");
}

veiculo ler_veiculo(FILE *stream, int tamRegistro){
    veiculo v;
    v.cidade = NULL;
    v.marca = NULL;
    v.modelo = NULL;

    // campos de tamanho fixo
    fread(&(v.id), sizeof(int), 1, stream);
    fread(&(v.ano), sizeof(int), 1, stream);
    fread(&(v.qtt), sizeof(int), 1, stream);
    fread(v.sigla, sizeof(char), 2, stream);
    int tamAtual = sizeof(int) + sizeof(int) + sizeof(int) + (sizeof(char) * 2);

    // campos de tamanho variavel
    char c;
    while ((c = fgetc(stream)) != '$' && tamAtual < tamRegistro){
        ungetc(c, stream);

        int tam;
        char codigo;
        fread(&tam, sizeof(int), 1, stream);
        fread(&codigo, sizeof(char), 1, stream);

        if (codigo == '0'){
            v.cidade = malloc((tam + 1) * sizeof(char));
            fread(v.cidade, sizeof(char), tam, stream);
            v.cidade[tam] = '\0';
        }
        else if (codigo == '1'){
            v.marca = malloc((tam + 1) * sizeof(char));
            fread(v.marca, sizeof(char), tam, stream);
            v.marca[tam] = '\0';
        }
        else if (codigo == '2'){
            v.modelo = malloc((tam + 1) * sizeof(char));
            fread(v.modelo, sizeof(char), tam, stream);
            v.modelo[tam] = '\0';
        }
        tamAtual += sizeof(int) + sizeof(char) + tam;
    }
    ungetc(c, stream);

    // posicionar a leitura do arquivo no proximo registro
    if (tamAtual < tamRegistro) fseek(stream, tamRegistro - tamAtual, SEEK_CUR);

    return v;
}

void escrever_veiculo(veiculo v, FILE *stream){
    // campos de tamanho fixo
    fwrite(&v.id, sizeof(int), 1, stream);
    fwrite(&v.ano, sizeof(int), 1, stream);
    fwrite(&v.qtt, sizeof(int), 1, stream);
    fwrite(v.sigla, sizeof(char), 2, stream);

    // campos de tamanho variavel
    if (v.cidade){
        int tamCidade = strlen(v.cidade);
        char codigo = '0';
        fwrite(&tamCidade, sizeof(int), 1, stream);
        fwrite(&codigo, sizeof(char), 1, stream);
        fwrite(v.cidade, sizeof(char), tamCidade, stream);
    }
    if (v.marca){
        int tamMarca = strlen(v.marca);
        char codigo = '1';
        fwrite(&tamMarca, sizeof(int), 1, stream);
        fwrite(&codigo, sizeof(char), 1, stream);
        fwrite(v.marca, sizeof(char), tamMarca, stream);
    }
    if (v.modelo){
        int tamModelo = strlen(v.modelo);
        char codigo = '2';
        fwrite(&tamModelo, sizeof(int), 1, stream);
        fwrite(&codigo, sizeof(char), 1, stream);
        fwrite(v.modelo, sizeof(char), tamModelo, stream);
    }
}

int calcular_tamanho(veiculo v){
    int tamanho = sizeof(int) + sizeof(int) + sizeof(int) + (sizeof(char) * 2);
    if (v.cidade)
        tamanho += sizeof(char) + sizeof(int) + strlen(v.cidade);
    if (v.marca)
        tamanho += sizeof(char) + sizeof(int) + strlen(v.marca);
    if (v.modelo)
        tamanho += sizeof(char) + sizeof(int) + strlen(v.modelo);
    return tamanho;
}

void desalocar_veiculo(veiculo v){
    if (v.cidade)
        free(v.cidade);
    if (v.marca)
        free(v.marca);
    if (v.modelo)
        free(v.modelo);
}

veiculo ler_veiculo_csv(FILE *stream){
    veiculo v;
    char *id;
    fscanf(stream, "%m[^,]", &id);
    fgetc(stream);
    v.id = atoi(id);
    char *ano;
    fscanf(stream, "%m[^,]", &ano);
    fgetc(stream);
    v.ano = ano ? atoi(ano) : -1;
    fscanf(stream, "%m[^,]", &v.cidade);
    fgetc(stream);
    char *qtt;
    fscanf(stream, "%m[^,]", &qtt);
    fgetc(stream);
    v.qtt = qtt ? atoi(qtt) : -1;
    char *sigla;
    fscanf(stream, "%m[^,]", &sigla);
    fgetc(stream);
    strncpy(v.sigla, sigla ? sigla : "$$", 2);
    fscanf(stream, "%m[^,]", &v.marca);
    fgetc(stream);
    fscanf(stream, "%m[^\r\n]", &v.modelo);
    linebreak(stream);
    
    free(id);
    free(ano);
    free(qtt);
    free(sigla);
    return v;
}

long int filtrarVeiculo(FILE *stream, veiculo f, char tipo, veiculo *v, long int *next){
    // guarda o offset do inicio do registro em relacao ao arquivo inteiro
    long int inicio = ftell(stream);
    // guarda o tamanho dos bytes lidos do registro ate o momento
    int lido = 0;
    // guarda o tamanho total do registro em bytes
    int tamRegistro;

    // campos de tamanho fixo
    char removido;
    fread(&removido, sizeof(char), 1, stream);
    lido++;
    if(tipo == '1'){
        // para o tipo1 os registros tem sempre 97 bytes
        tamRegistro = TAM_TIPO1;
        if(removido == '1'){
            *next = tamRegistro - lido;
            return -1;
        }
        // ignora o campo de proximo arquivo removido ja que nao lidaremos com
        // arquivos removidos
        fseek(stream, sizeof(int), SEEK_CUR);
        lido += sizeof(int);
    }
    else if(tipo == '2'){
        // para o tipo2 o tamanho total esta armazenado no inicio do registro
        fread(&tamRegistro, sizeof(int), 1, stream);
        lido += sizeof(int);
        // o tamanho armazenado no inicio do registro nao considera os 4 bytes
        // do proprio campo nem o byte do campo de identificador de registros
        // removido mas o funcionamento dessa funcao requer que sejam levados
        // em consideracao todos os bytes do registro portanto somamos 5
        tamRegistro += sizeof(int) + sizeof(char);
        if(removido == '1'){
            *next = tamRegistro - lido;
            return -1;
        }
        // para o tipo2 o campo de proximo arquivo removido e um long int
        fseek(stream, sizeof(long), SEEK_CUR);
        lido += sizeof(long);
    }
    fread(&v->id, sizeof(int), 1, stream);
    lido += sizeof(int);
    // se o filtro exigir o campo id compara o id do registro com o do filtro
    if((f.id != -1) && (v->id != f.id)){
        *next = tamRegistro - lido;
        return -1;
    }
    fread(&v->ano, sizeof(int), 1, stream);
    lido += sizeof(int);
    // se o filtro exigir o campo ano compara o ano do registro com o do filtro
    if((f.ano != -1) && (v->ano != f.ano)){
        *next = tamRegistro - lido;
        return -1;
    }
    fread(&v->qtt, sizeof(int), 1, stream);
    lido += sizeof(int);
    // se o filtro exigir o campo qtt compara o qtt do registro com o do filtro
    if((f.qtt != -1) && (v->qtt != f.qtt)){
        *next = tamRegistro - lido;
        return -1;
    }
    lido += fread(v->sigla, sizeof(char), 2, stream);
    // se o filtro exigir o campo sigla compara a do registro com a do filtro
    if(strncmp(f.sigla, "$$", 2) && strncmp(v->sigla, f.sigla, 2)){
        *next = tamRegistro - lido;
        return -1;
    }

    // campos de tamanho variavel
    v->cidade = NULL;
    v->marca = NULL;
    v->modelo = NULL;
    // se tiver chegado ao final do registro checa se o filtro exige algum dos
    // campos que ainda nao foram lidos
    if((lido >= tamRegistro) && (f.cidade || f.marca || f.modelo)){
        *next = tamRegistro - lido;
        return -1;
    }
    char c = fgetc(stream);
    if((ungetc(c, stream) == '$') && (f.cidade || f.marca || f.modelo)){
        *next = tamRegistro - lido;
        return -1;
    }
    // guarda o tamanho do proximo campo variavel em bytes
    int tam;
    // guarda o tipo do proximo campo variavel
    char codigo;
    // se tiver chegado ao final do registro nao tenta ler proximos campos
    if(lido >= tamRegistro){
        codigo = -1;
    }
    else{
        fread(&tam, sizeof(int), 1, stream);
        lido += sizeof(int);
        lido += fread(&codigo, sizeof(char), 1, stream);
    }
    // codigo 0 se refere ao campo cidade
    if(codigo == '0'){
        // aloca 1 byte a mais que o tamanho informado para o \0
        v->cidade = malloc((tam + 1) * sizeof(char));
        lido += fread(v->cidade, sizeof(char), tam, stream);
        v->cidade[tam] = '\0';
        // se o filtro exigir o campo cidade compara com o do registro
        // se tiver chegado ao final do registro checa se o filtro exige algum
        // dos campos que ainda nao foram lidos
        if((f.cidade && strcmp(v->cidade, f.cidade)) || ((lido >= tamRegistro) && (f.marca || f.modelo))){
            *next = tamRegistro - lido;
            return -1;
        }
        char c = fgetc(stream);
        if((ungetc(c, stream) == '$') && (f.marca || f.modelo)){
            *next = tamRegistro - lido;
            return -1;
        }
        // como o campo cidade foi lido passa para o proximo campo se nao tiver chegado ao final do arquivo
        if(lido >= tamRegistro){
            codigo = -1;
        }
        else{
            fread(&tam, sizeof(int), 1, stream);
            lido += sizeof(int);
            lido += fread(&codigo, sizeof(char), 1, stream);
        }
    }
    else{
        // se nao houver campo cidade checa se o filtro exige esse campo
        if(f.cidade){
            *next = tamRegistro - lido;
            return -1;
        }
    }
    // codigo 1 se refere ao campo marca
    if(codigo == '1'){                                                                                                     
        v->marca = malloc((tam + 1) * sizeof(char));
        lido += fread(v->marca, sizeof(char), tam, stream);
        v->marca[tam] = '\0';
        if((f.marca && strcmp(v->marca, f.marca)) || ((lido >= tamRegistro) && f.modelo)){
            *next = tamRegistro - lido;
            return -1;
        }
        char c = fgetc(stream);
        if((ungetc(c, stream) == '$') && f.modelo){
            *next = tamRegistro - lido;
            return -1;
        }
        if(lido >= tamRegistro){
            codigo = -1;
        }
        else{
            fread(&tam, sizeof(int), 1, stream);
            lido += sizeof(int);
            lido += fread(&codigo, sizeof(char), 1, stream);
        }
    }
    else{
        if(f.marca){
            *next = tamRegistro - lido;
            return -1;
        }
    }
    // codigo 2 se refere ao campo modelo
    if(codigo == '2'){
        v->modelo = malloc((tam + 1) * sizeof(char));
        lido += fread(v->modelo, sizeof(char), tam, stream);
        v->modelo[tam] = '\0';
        if(f.modelo && strcmp(v->modelo, f.modelo)){
            *next = tamRegistro - lido;
            return -1;
        }
    }
    else{
        if(f.modelo){
            *next = tamRegistro - lido;
            return -1;
        }
    }
    *next = tamRegistro - lido;
    return inicio;
}

veiculo ler_novo_veiculo(FILE *stream){
    veiculo v;
    v.cidade = NULL;
    v.marca = NULL;
    v.modelo = NULL;

    // le os dados como strings
    char *id, *ano, *qtt, *sigla, *cidade, *marca, *modelo;
    fscanf(stream, "%ms", &id);
    fscanf(stream, "%ms", &ano);
    fscanf(stream, "%ms", &qtt);
    sigla = scan_quote_string();
    cidade = scan_quote_string();
    marca = scan_quote_string();
    modelo = scan_quote_string();

    // converte os dados numericos para inteiros
    v.id = (strcmp(id, "NULO") == 0) ? -1 : atoi(id);
    v.ano = (strcmp(ano, "NULO") == 0) ? -1 : atoi(ano);
    v.qtt = (strcmp(qtt, "NULO") == 0) ? -1 : atoi(qtt);

    // adiciona lixo no espaco da sigla caso esteja vazia
    if (strcmp(sigla, "") == 0) strncpy(v.sigla, "$$", 2);
    else strncpy(v.sigla, sigla, 2);

    if (strcmp(cidade, "") != 0) v.cidade = cidade;
    else free(cidade);

    if (strcmp(marca, "") != 0) v.marca = marca;
    else free(marca);

    if (strcmp(modelo, "") != 0) v.modelo = modelo;
    else free(modelo);

    free(id);
    free(ano);
    free(qtt);
    free(sigla);
    return v;
}

long int buscar_veiculo(FILE *stream, void *indices, int qtd_ind, veiculo f, char tipo, long int *next){
    // se a busca usar id ela deve ser feita pela lista de indices
    if (f.id != -1){
        int pos = busca_indices(indices, 0, qtd_ind, f.id);
        if(pos == -1) return -1;
        // calcula o offset do registro dependendo do tipo
        long offset = (tipo == '1') ? TAM_CAB1 + ((Indice*)indices)[pos].RRN * TAM_TIPO1 : ((Indice*)indices)[pos].byteOffset;
        // salva a posicao atual do ponteiro do arquivo de dados para poder voltar para ela no final da busca
        long cur = ftell(stream);
        fseek(stream, offset, SEEK_SET);
        veiculo v = {-1, -1, -1, "$$", NULL, NULL, NULL};
        // checa se o registro do indice encontrado coincide com os outros campos da busca
        long filtro = filtrarVeiculo(stream, f, tipo, &v, next);
        desalocar_veiculo(v);
        // retorna o ponteiro do arquivo a posicao antes da busca
        fseek(stream, cur, SEEK_SET);
        return filtro;
    }
    
    // se a busca nao usar id ela e feita sequencialmente pelo arquivo de dados
    char c = fgetc(stream);
    while (!feof(stream)){
        ungetc(c, stream);
        veiculo v = {-1, -1, -1, "$$", NULL, NULL, NULL};
        long int cur = filtrarVeiculo(stream, f, tipo, &v, next);
        desalocar_veiculo(v);
        // retorna o primeiro registro que coincidir com os campos da busca
        if(cur != -1) return cur;
        if (*next) fseek(stream, *next, SEEK_CUR);
        c = fgetc(stream);
    }
    return -1;
}

void atualizar_veiculo(veiculo *v, veiculo *valores, veiculo *campos){
    // valores de tamanho fixo
    if (campos->id != -1)  v->id = valores->id;
    if (campos->ano != -1) v->ano = valores->ano;
    if (campos->qtt != -1) v->qtt = valores->qtt;
    if (strncmp(campos->sigla, "$$", 2)) strncpy(v->sigla, valores->sigla, 2);
    // valores de tamanho variavel
    if (campos->cidade){
        if(v->cidade){
            free(v->cidade);
            v->cidade = NULL;
        }
        if(valores->cidade){
            v->cidade = realloc(v->cidade, (strlen(valores->cidade) + 1) * sizeof(char));
            // se o campo deve ser atualizado e se foi dado um novo valor para ele copia o valor para o objeto
            strcpy(v->cidade, valores->cidade);
        }
    }
    if (campos->marca){
        if(v->marca){
            free(v->marca);
            v->marca = NULL;
        }
        if(valores->marca){
            v->marca = realloc(v->marca, (strlen(valores->marca) + 1) * sizeof(char));
            strcpy(v->marca, valores->marca);
        }
    }
    if (campos->modelo){
        if(v->modelo){
            free(v->modelo);
            v->modelo = NULL;
        }
        if(valores->modelo){
            v->modelo = realloc(v->modelo, (strlen(valores->modelo) + 1) * sizeof(char));
            strcpy(v->modelo, valores->modelo);
        }
    }
}

void remover_veiculo(FILE *bin, long cur, char tipo, void *rc){
    fseek(bin, cur, SEEK_SET);
    char rem = '1';
    // define o registro como logicamente removido
    fwrite(&rem, sizeof(char), 1, bin);
    if(tipo == '1'){
        // se for tipo 1 escreve o RRN no topo da pilha no campo de proximo RRN deletado do registro
        fwrite(&((cabecalho *)rc)->topo1, sizeof(int), 1, bin);
        // substitui o topo da pilha pelo RRN do ultimo registro
        ((cabecalho *)rc)->topo1 = (cur - TAM_CAB1) / TAM_TIPO1;
    }
    else if(tipo == '2'){
        int tam;
        fread(&tam, sizeof(int), 1, bin);
        // salva o offset do campo de proximo offset removido
        long prox = ftell(bin);
        long ant = -1;
        long offset;
        if(((cabecalho *)rc)->topo2 != -1){
            fseek(bin, ((cabecalho *)rc)->topo2 + 1, SEEK_SET);
            int tam2;
            fread(&tam2, sizeof(int), 1, bin);
            // se o topo da lista ordenada nao estiver vazio procura o proximo registro deletado menor que o ultimo
            while(tam < tam2){
                ant = ftell(bin);
                fread(&offset, sizeof(long), 1, bin);
                fseek(bin, offset + 1, SEEK_SET);
                fread(&tam2, sizeof(int), 1, bin);
            }
        }
        if(ant == -1){
            // subtitui apenas o topo
            fseek(bin, prox, SEEK_SET);
            fwrite(&((cabecalho *)rc)->topo2, sizeof(long), 1, bin);
            ((cabecalho *)rc)->topo2 = cur;
        }
        else{
            // refaz a ligacao dos registros removidos para incluir o novo registro no meio
            fseek(bin, ant, SEEK_SET);
            fwrite(&cur, sizeof(long), 1, bin);
            fseek(bin, prox, SEEK_SET);
            fwrite(&offset, sizeof(long), 1, bin);
        }
    }
    ((cabecalho *)rc)->nroRegRem++;
}

void inserir_veiculo(void *rc, FILE *bin, veiculo v){
    int tamTopo = -1;
    char removido = '0';
    long prox = -1;
    if (((cabecalho *)rc)->topo2 != -1){
        fseek(bin, ((cabecalho *)rc)->topo2 + 1, SEEK_SET);
        fread(&tamTopo, sizeof(int), 1, bin);
    }
    int tamRegistro = sizeof(long) + calcular_tamanho(v);
    // nao existem registros removidos ou espaco insuficiente -> escrever no fim do arquivo
    if (((cabecalho *)rc)->topo2 == -1 || tamRegistro > tamTopo){
        fseek(bin, ((cabecalho *)rc)->proxByteOffset, SEEK_SET);
        fwrite(&removido, sizeof(char), 1, bin);
        fwrite(&tamRegistro, sizeof(int), 1, bin);
        fwrite(&prox, sizeof(long), 1, bin);
        escrever_veiculo(v, bin);
        ((cabecalho *)rc)->proxByteOffset += sizeof(char) + sizeof(int) + tamRegistro;
    }
    // existe registro removido com espaco suficiente -> escrever no byteOffset
    else{
        fseek(bin, ((cabecalho *)rc)->topo2, SEEK_SET);
        fwrite(&removido, sizeof(char), 1, bin);
        fseek(bin, 4, SEEK_CUR);
        fread(&((cabecalho *)rc)->topo2, sizeof(long), 1, bin);
        fseek(bin, -8, SEEK_CUR);
        fwrite(&prox, sizeof(long), 1, bin);
        escrever_veiculo(v, bin);
        ((cabecalho *)rc)->nroRegRem--;
        
        // preencher resto do registro com lixo
        char lixo = '$';
        while (tamRegistro < tamTopo){
            fwrite(&lixo, sizeof(char), 1, bin);
            tamRegistro += sizeof(char);
        }
    }
}