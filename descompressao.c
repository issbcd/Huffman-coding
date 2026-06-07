#include "descompressao.h"
#include "compressao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Para a descompactação precisamos:
abrir o arquivo compactado
ler o cabeçalho e extrair o tamanho do lixo e o tamanho da arvore
reconstruir a arvore
ler os bytes restantes navegando pela arvore ate mapear as "letras" originais
salvar o arquivo descompactado
*/

/*-------------------parte 6: decodificar e salvar arquivo--------------------*/

/*aqui a gente le a sequencia em pre ordem e reconstrói a arvore*/
no* reconstruir_arvore(FILE *entrada, int *bytes_arvore){
    if(*bytes_arvore <= 0) //caso base: se todos os bytes da arvore foram lidos, interrompe
    {
        return NULL;
    }
    
    //le o proximo caractere do arquivo para identificar o prox no
    int caracter = fgetc(entrada);
    (*bytes_arvore)--; //diminui o contador, ja que foi lido um byte
    
    if(caracter == '\\') //se o caracter for barra invertida o carctere "real" é o proximo
    {
        caracter = fgetc(entrada); //pega o proximo caractere
        (*bytes_arvore)--; //diminui o contador, ja que foi lido mais um byte
    }

    else if(caracter == '*'){
        //se o caractere for '*' é um no interno
        no *pai = (no*)malloc(sizeof(no)); //aloca espaço para no interno/pai
        if(pai == NULL)
        {
            printf("Erro ao alocar no pai na reconstrucao.\n");
            exit(1);
        }

        pai->byte = (unsigned char*)malloc(sizeof(unsigned char)); //aloca espaço para o caractere
        if(pai->byte == NULL) 
        {
            printf("Erro ao alocar byte pai na reconstrucao.\n");
            exit(1);
        }
        *(unsigned char*)(pai->byte) = '*';
        pai->frequencia = 0;

        pai->esq = NULL;
        pai->dir = NULL;

        /*passo recursivo, o filho a esquerda é reconstruido primeiro e continua a leitura do arquivo
        apos terminar o esquerdo faz a recursao a direita*/
        pai->esq = reconstruir_arvore(entrada, bytes_arvore);
        pai->dir = reconstruir_arvore(entrada, bytes_arvore);

        return pai; //retorna o pai com seus filhos
    }

    /*se passou pelos if's é uma folha, o caracter representa uma letra original do alfabeto*/
    no *folha = (no*)malloc(sizeof(no)); //aloca espaço para a folha
    folha->byte = malloc(sizeof(unsigned char)); //aloca espaço para o caractere
    //armazena o caractere na folha
    *(unsigned char*)(folha->byte) = (unsigned char)caracter; //converte o caractere para unsigned char
    folha->frequencia = 0; //frequencia e NULL são definidos
    folha->esq = NULL; //é folha
    folha->dir = NULL; //é folha

    return folha; //retorna a folha
}
/*abre o arquivo e faz as operações bitwise para extrair os dados do cabeçalho e decodificar o arquivo*/
void decodificar_arquivo(char *arquivo_huff){
    FILE *entrada = fopen(arquivo_huff, "rb");
    if(entrada == NULL) {
        printf("Erro ao abrir arquivo.\n");
        exit(1);
    }

    unsigned char byte1 = fgetc(entrada);
    unsigned char byte2 = fgetc(entrada);

    //bitwise - extração dos dados do cabeçalho
    int tamanho_lixo = byte1 >> 5; //desloca o primeiro byte 5 posiçoes a direita, isolando os 3 bits que indicam o tamanho do lixo
    int tamanho_arvore = ((byte1 & 31) << 8) | byte2; //aplica mascara de 31 (00011111) no byte 1 para limpar os bits do lixo e desloca os 5 bits restantes 8 posicoes para a esquerda, faz a operação "ou" que resulta no tamanho da arvore

    int contador_arvore = tamanho_arvore; //variavel de controle
    no *raiz_arvore = reconstruir_arvore(entrada, &contador_arvore); //reconstroi a arvore

    char nome_saida[256];
    strncpy(nome_saida, arquivo_huff, strlen(arquivo_huff) - 5); //substitui o .huff por .descompactado
    nome_saida[strlen(arquivo_huff) - 5] = '\0'; //adiciona o terminador nulo
    
    FILE *saida = fopen(nome_saida, "wb");
    if (saida == NULL) {
        printf("Erro ao criar arquivo de saída.\n");
        fclose(entrada);
        exit(1);
    }
    /*decodificação dos bits*/
    no *atual = raiz_arvore; //ponteiro auxiliar para percorrer a arvore
    int prox_bit = fgetc(entrada);
    int byte_lido;

    while((byte_lido = prox_bit) != EOF){
        prox_bit = fgetc(entrada);//tenta ler o bit seguinte para verificar se o atual é o ultimo

        int limite_bits;
        if(prox_bit == EOF){
            //se for o ultimo byte do arquivo, processa somente os bits q nao sao lixo
            limite_bits = 8 - tamanho_lixo;
        }
        else{
            limite_bits = 8;
        }

        for(int i = 0; i < limite_bits; i++){
            unsigned char mascara = 128 >> i;
            if(byte_lido & mascara){
                atual = atual->dir;
            }
            else
            {
                atual = atual->esq;
            }
            if(atual->esq == NULL && atual->dir == NULL){
                fputc(*(unsigned char*)(atual->byte), saida);
                atual = raiz_arvore;
            }
        }
        
    }
    printf("Arquivo descompactado com sucesso! Gerado: %s\n", nome_saida);
    fclose(entrada);
    fclose(saida);
    liberar_arvore(raiz_arvore);

    
}