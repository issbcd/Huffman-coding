#include "descompressao.h"
#include "compressao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @file descompressao.c
 * @brief Implementação das funções responsáveis pela reconstrução da árvore de Huffman e decodificação do arquivo compactado.
 */

/*------------------- PARTE 6: DECODIFICAR E SALVAR ARQUIVO --------------------*/

/**
 * Reconstrói a árvore de Huffman recursivamente a partir da sequência gravada em pré-ordem no arquivo.
 * * @param entrada Ponteiro para o arquivo compactado (.huff) posicionado no início da árvore.
 * @param bytes_arvore Ponteiro para o contador de controle de bytes restantes da árvore.
 * @return no* Ponteiro para a raiz da subárvore ou árvore reconstruída. Retorna NULL ao finalizar os bytes.
 */
no* reconstruir_arvore(FILE *entrada, int *bytes_arvore){
    if(*bytes_arvore <= 0) 
    {
        return NULL;
    }
    
    int caracter = fgetc(entrada);
    (*bytes_arvore)--; 
    
    if(caracter == '\\') 
    {
        caracter = fgetc(entrada); 
        (*bytes_arvore)--; 
    }
    else if(caracter == '*'){
        no *pai = (no*)malloc(sizeof(no)); 
        if(pai == NULL)
        {
            printf("Erro ao alocar no pai na reconstrucao.\n");
            exit(1);
        }

        pai->byte = (unsigned char*)malloc(sizeof(unsigned char)); 
        if(pai->byte == NULL) 
        {
            printf("Erro ao alocar byte pai na reconstrucao.\n");
            exit(1);
        }
        *(unsigned char*)(pai->byte) = '*';
        pai->frequencia = 0;
        pai->esq = NULL;
        pai->dir = NULL;

        pai->esq = reconstruir_arvore(entrada, bytes_arvore);
        pai->dir = reconstruir_arvore(entrada, bytes_arvore);

        return pai; 
    }

    no *folha = (no*)malloc(sizeof(no)); 
    folha->byte = malloc(sizeof(unsigned char)); 
    
    *(unsigned char*)(folha->byte) = (unsigned char)caracter; 
    folha->frequencia = 0; 
    folha->esq = NULL; 
    folha->dir = NULL; 

    return folha; 
}

/**
 * Abre o arquivo compactado, extrai os metadados do cabeçalho via operações bitwise,
 * reconstrói a árvore e decodifica a sequência de bits salvando o arquivo original de volta.
 * * @param arquivo_huff String contendo o caminho ou nome do arquivo comprimido (.huff).
 */
void decodificar_arquivo(char *arquivo_huff){
    FILE *entrada = fopen(arquivo_huff, "rb");
    if(entrada == NULL) {
        printf("Erro ao abrir arquivo.\n");
        exit(1);
    }

    unsigned char byte1 = fgetc(entrada);
    unsigned char byte2 = fgetc(entrada);

    int tamanho_lixo = byte1 >> 5; 
    int tamanho_arvore = ((byte1 & 31) << 8) | byte2; 

    int contador_arvore = tamanho_arvore; 
    no *raiz_arvore = reconstruir_arvore(entrada, &contador_arvore); 

    char nome_saida[256];
    strncpy(nome_saida, arquivo_huff, strlen(arquivo_huff) - 5); 
    nome_saida[strlen(arquivo_huff) - 5] = '\0'; 
    
    FILE *saida = fopen(nome_saida, "wb");
    if (saida == NULL) {
        printf("Erro ao criar arquivo de saída.\n");
        fclose(entrada);
        exit(1);
    }

    no *atual = raiz_arvore; 
    int prox_bit = fgetc(entrada);
    int byte_lido;

    while((byte_lido = prox_bit) != EOF){
        prox_bit = fgetc(entrada);

        int limite_bits;
        if(prox_bit == EOF){
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