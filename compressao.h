#ifndef COMPRESSAO_H
#define COMPRESSAO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TAM 256

/*vms criar a struct no, vai funcionar como nó da arvore (esquerda/direita) e da lista(proximo)*/
typedef struct no{
    void *byte; /*o ponteiro é generico e vai aceitar td tipo*/
    int frequencia; /*vai marcar quantas vzs o byte vai aparecer no arquivo e é o criterio de prioridade*/
    struct no *esq;/*filho da esquerda - bit 0*/
    struct no *dir;/*filho da direita - bit 1*/
}no;

/*struct da lista encadeada ordenada*/
typedef struct heap{
    no **dados;//array de ponteiros para nó
    int tamanho;
    int capacidade;
}heap;


/*funcoes de leitura e frequencia*/
void inicializa_tabela_com_zero(unsigned int *tab);
void readfile(char *filename, unsigned int *frequencia);

/*as funcoes da heap e do nó*/
heap *criar_heap(int capacidade);/*aloca espaço e inicializa a heap*/
void sobe(heap *h, int i);/*aqui atualizaremos a frequencia ou ent add um nó na lista, se o byte ja existe, add frequencia. se ele nn existir, cria um nó e o **head indica q pode modificar o inicio da lista, se for preciso*/
void heap_insere(heap *h, no *novo_no);/*preenche a heap com os bytes e suas frequencias*/
no *heap_retira_min(heap *h);/*remove e retorna o primeiro no da heap*/

/*funcoes da arvore*/
no *montar_arvore(heap *h);/*enquanto a heap tiver mais de um elemento vai retirar os dois de menor frequencia e cria um nó pai com a frequencia, no final, retorna a raiz. vai unir os nos da heap p formar a arvore final*/
int altura_arvore(no *raiz);/*calcula a altura da arvore*/
void preencher_heap(heap *h, unsigned int *tab_frequencia);
void desce(heap *h, int i);
void liberar_arvore(no *raiz);/*limpa a arvore da memoria, percorre a arvore em pós ordem e da esse free*/
void liberar_heap(heap *h);

/*funcoes do dicionario*/
char **aloca_dicionario(int colunas);/*aloca dinamicamente a matriz do dicionario*/
void gerar_dicionario(no *raiz, char **dicionario, int nivel, char *caminho);/*percorre de modo recursivo e gera o codigo em cada folha, chegou na folha, salva o par e a profundidade guia a posicao do array, o pontiero i indica o indice atual da tabela*/

/*as funcoes de compressao*/
/*vms precisar guardar o desenho da arvore na saida, criar um dicionario de bits, transformar o texto em bits e gravar no arquivo e gravar as infos do lixo e o tamanho, no inicio*/
int calcular_tamanho_arvore(no *raiz);/*calcula o tamanho da arvore em bytes*/
void pre_ordem_arvore(no *raiz, FILE *saida);/*usada pra q a descompressao consiga reconstruir a arvore e decodificar os bits*/
int calcula_tam_string(char **dicionario, FILE *arquivo);/*le o arquivo do inicio ao fim e calcula o tam total q a string de '0' e '1' irá ocupar na memoria*/
void compactar(char **dicionario, char *nome_arquivo, no *arvore, unsigned int *tab_frequencia);/*busca o codigo de cada byte original na tabela e escreve os bits correspondentes, o ponteiro lixo conta quantos bits do ultimo byte sao de preenchimento, é preciso p saber onde a parte util acaba*/

#endif
