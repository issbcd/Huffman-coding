#ifndef compressao.h
#define compressao.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TAM 256

/*vms criar a struct no, vai funcionar como nó da arvore (esquerda/direita) e da lista(proximo)*/
typedef struct no{
    void *byte; /*o ponteiro é generico e vai aceitar td tipo*/
    int frequencia; /*vai marcar quantas vzs o byte vai aparecer no arquivo e é o criterio de prioridade*/
    struct no *proximo;/*aponta p prox nó da lista, vms usar p montar a heap*/
    struct no *esq;/*filho da esquerda - bit 0*/
    struct no *dir;/*filho da direita - bit 1*/
}no;

/*struct da lista encadeada ordenada*/
typedef struct lista{
    no *inicio;
    int tamanho;
}lista;

/*agr, uma struct q vai guardar o binario de cada byte e criar a tabela*/
typedef struct tabela_huff{
    void *byte; /*aqui vms chamar o caractere do qual o codigo se encontra*/
    char codigo[TAM]; /*aqui temos a string q vai guardar o codigo binario, seu tamanho max é 256*/
}tabela_huff;

/*funcoes de leitura e frequencia*/
void inicializa_tabela_com_zero(unsigned int *tab);
void readfile(char *filename, unsigned int *frequencia);

/*as funcoes da lista e do nó*/
void criar_lista(lista *lista);/*aloca espaço e inicializa a lista*/
void inserir_ordenado(lista *lista, no *novo_no);/*aqui atualizaremos a frequencia ou ent add um nó na lista, se o byte ja existe, add frequencia. se ele nn existir, cria um nó e o **head indica q pode modificar o inicio da lista, se for preciso*/
void preencher_lista_ordenada(lista *lista, unsigned int *tab_frequencia);/*preenche a lista com os bytes e suas frequencias*/
no *remove_inicio_lista(lista *lista);/*remove e retorna o primeiro no da lista*/

/*funcoes da arvore*/
no *montar_arvore(lista *lista);/*enquanto a heap tiver mais de um elemento vai retirar os dois de menor frequencia e cria um nó pai com a frequencia, no final, retorna a raiz. vai unir os nos da heap p formar a arvore final*/
int altura_arvore(no *raiz);/*calcula a altura da arvore*/
void liberar_arvore(no *raiz);/*limpa a arvore da memoria, percorre a arvore em pós ordem e da esse free*/

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
