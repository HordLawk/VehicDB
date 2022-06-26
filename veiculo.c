#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veiculo.h"
#include "utils.h"

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
    fread(&(v.id), 4, 1, stream);
    fread(&(v.ano), 4, 1, stream);
    fread(&(v.qtt), 4, 1, stream);
    fread(v.sigla, 1, 2, stream);
    int tamAtual = 4 + 4 + 4 + 2;

    // campos de tamanho variavel
    char c;
    while ((c = fgetc(stream)) != '$' && tamAtual < tamRegistro){
        ungetc(c, stream);

        int tam;
        char codigo;
        fread(&tam, 4, 1, stream);
        fread(&codigo, 1, 1, stream);

        if (codigo == '0'){
            v.cidade = malloc((tam + 1) * sizeof(char));
            fread(v.cidade, 1, tam, stream);
            v.cidade[tam] = '\0';
        }
        else if (codigo == '1'){
            v.marca = malloc((tam + 1) * sizeof(char));
            fread(v.marca, 1, tam, stream);
            v.marca[tam] = '\0';
        }
        else if (codigo == '2'){
            v.modelo = malloc((tam + 1) * sizeof(char));
            fread(v.modelo, 1, tam, stream);
            v.modelo[tam] = '\0';
        }
        tamAtual += 4 + 1 + tam;
    }
    ungetc(c, stream);

    // posicionar a leitura do arquivo no proximo registro
    if (tamAtual < tamRegistro) fseek(stream, tamRegistro - tamAtual, SEEK_CUR);

    return v;
}

void escrever_veiculo(veiculo v, FILE *stream){
    // campos de tamanho fixo
    fwrite(&v.id, 4, 1, stream);
    fwrite(&v.ano, 4, 1, stream);
    fwrite(&v.qtt, 4, 1, stream);
    fwrite(v.sigla, 1, 2, stream);

    // campos de tamanho variavel
    if (v.cidade){
        int tamCidade = strlen(v.cidade);
        char codigo = '0';
        fwrite(&tamCidade, 4, 1, stream);
        fwrite(&codigo, 1, 1, stream);
        fwrite(v.cidade, 1, tamCidade, stream);
    }
    if (v.marca){
        int tamMarca = strlen(v.marca);
        char codigo = '1';
        fwrite(&tamMarca, 4, 1, stream);
        fwrite(&codigo, 1, 1, stream);
        fwrite(v.marca, 1, tamMarca, stream);
    }
    if (v.modelo){
        int tamModelo = strlen(v.modelo);
        char codigo = '2';
        fwrite(&tamModelo, 4, 1, stream);
        fwrite(&codigo, 1, 1, stream);
        fwrite(v.modelo, 1, tamModelo, stream);
    }
}

int calcular_tamanho(veiculo v){
    int tamanho = 4 + 4 + 4 + 2;
    if (v.cidade)
        tamanho += 1 + 4 + strlen(v.cidade);
    if (v.marca)
        tamanho += 1 + 4 + strlen(v.marca);
    if (v.modelo)
        tamanho += 1 + 4 + strlen(v.modelo);
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

int filtrarVeiculo(FILE *stream, veiculo f, char tipo, veiculo *v){
    // guarda o tamanho dos bytes lidos do registro ate o momento
    int lido = 0;
    // guarda o tamanho total do registro em bytes
    int tamRegistro;

    // campos de tamanho fixo
    char removido;
    fread(&removido, 1, 1, stream);
    lido++;
    if(tipo == '1'){
        // para o tipo1 os registros tem sempre 97 bytes
        tamRegistro = 97;
        if(removido == '1') return tamRegistro - lido;
        // ignora o campo de proximo arquivo removido ja que nao lidaremos com
        // arquivos removidos
        fseek(stream, 4, SEEK_CUR);
        lido += 4;
    }
    else if(tipo == '2'){
        // para o tipo2 o tamanho total esta armazenado no inicio do registro
        fread(&tamRegistro, 4, 1, stream);
        lido += 4;
        // o tamanho armazenado no inicio do registro nao considera os 4 bytes
        // do proprio campo nem o byte do campo de identificador de registros
        // removido mas o funcionamento dessa funcao requer que sejam levados
        // em consideracao todos os bytes do registro portanto somamos 5
        tamRegistro += 5;
        if(removido == '1') return tamRegistro - lido;
        // para o tipo2 o campo de proximo arquivo removido e um long int
        fseek(stream, 8, SEEK_CUR);
        lido += 8;
    }
    fread(&v->id, 4, 1, stream);
    lido += 4;
    // se o filtro exigir o campo id compara o id do registro com o do filtro
    if((f.id != -1) && (v->id != f.id)) return tamRegistro - lido;
    fread(&v->ano, 4, 1, stream);
    lido += 4;
    // se o filtro exigir o campo ano compara o ano do registro com o do filtro
    if((f.ano != -1) && (v->ano != f.ano)) return tamRegistro - lido;
    fread(&v->qtt, 4, 1, stream);
    lido += 4;
    // se o filtro exigir o campo qtt compara o qtt do registro com o do filtro
    if((f.qtt != -1) && (v->qtt != f.qtt)) return tamRegistro - lido;
    lido += fread(v->sigla, 1, 2, stream);
    // se o filtro exigir o campo sigla compara a do registro com a do filtro
    if(strncmp(f.sigla, "$$", 2) && strncmp(v->sigla, f.sigla, 2)) return tamRegistro - lido;

    // campos de tamanho variavel
    v->cidade = NULL;
    v->marca = NULL;
    v->modelo = NULL;
    // se tiver chegado ao final do registro checa se o filtro exige algum dos
    // campos que ainda nao foram lidos
    if((lido >= tamRegistro) && (f.cidade || f.marca || f.modelo)) return tamRegistro - lido;
    char c = fgetc(stream);
    if((ungetc(c, stream) == '$') && (f.cidade || f.marca || f.modelo)) return tamRegistro - lido;
    // guarda o tamanho do proximo campo variavel em bytes
    int tam;
    fread(&tam, 4, 1, stream);
    lido += 4;
    // guarda o tipo do proximo campo variavel
    char codigo;
    lido += fread(&codigo, 1, 1, stream);
    // codigo 0 se refere ao campo cidade
    if(codigo == '0'){
        // aloca 1 byte a mais que o tamanho informado para o \0
        v->cidade = malloc(tam + 1);
        lido += fread(v->cidade, 1, tam, stream);
        v->cidade[tam] = '\0';
        // se o filtro exigir o campo cidade compara com o do registro
        // se tiver chegado ao final do registro checa se o filtro exige algum
        // dos campos que ainda nao foram lidos
        if((f.cidade && strcmp(v->cidade, f.cidade)) || ((lido >= tamRegistro) && (f.marca || f.modelo))) return tamRegistro - lido;
        char c = fgetc(stream);
        if((ungetc(c, stream) == '$') && (f.marca || f.modelo)) return tamRegistro - lido;
        // como o campo cidade foi lido passa para o proximo campo
        fread(&tam, 4, 1, stream);
        lido += 4;
        lido += fread(&codigo, 1, 1, stream);
    }
    else{
        // se nao houver campo cidade checa se o filtro exige esse campo
        if(f.cidade) return tamRegistro - lido;
    }
    // codigo 1 se refere ao campo marca
    if(codigo == '1'){                                                                                                     
        v->marca = malloc(tam + 1);
        lido += fread(v->marca, 1, tam, stream);
        v->marca[tam] = '\0';
        if((f.marca && strcmp(v->marca, f.marca)) || ((lido >= tamRegistro) && f.modelo)) return tamRegistro - lido;
        char c = fgetc(stream);
        if((ungetc(c, stream) == '$') && f.modelo) return tamRegistro - lido;
        fread(&tam, 4, 1, stream);
        lido += 4;
        lido += fread(&codigo, 1, 1, stream);
    }
    else{
        if(f.marca) return tamRegistro - lido;
    }
    // codigo 2 se refere ao campo modelo
    if(codigo == '2'){
        v->modelo = malloc(tam + 1);
        lido += fread(v->modelo, 1, tam, stream);
        v->modelo[tam] = '\0';
        if(f.modelo && strcmp(v->modelo, f.modelo)) return tamRegistro - lido;
    }
    else{
        if(f.modelo) return tamRegistro - lido;
    }
    
    mostrar_veiculo(*v);
    return tamRegistro - lido;
}

veiculo ler_novo_veiculo(FILE *stream){
    veiculo v;
    v.cidade = NULL;
    v.marca = NULL;
    v.modelo = NULL;

    char *id, *ano, *qtt, *sigla, *cidade, *marca, *modelo;
    fscanf(stream, "%ms", &id);
    fscanf(stream, "%ms", &ano);
    fscanf(stream, "%ms", &qtt);
    sigla = scan_quote_string();
    cidade = scan_quote_string();
    marca = scan_quote_string();
    modelo = scan_quote_string();

    v.id = (strcmp(id, "NULO") == 0) ? -1 : atoi(id);
    v.ano = (strcmp(ano, "NULO") == 0) ? -1 : atoi(ano);
    v.qtt = (strcmp(qtt, "NULO") == 0) ? -1 : atoi(qtt);


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