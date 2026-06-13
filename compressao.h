#ifndef COMPRESSAO_H
#define COMPRESSAO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 256

/**
 * @file compressao.h
 * @brief Definições de estruturas e protótipos de funções para compressão de arquivos usando o algoritmo de Huffman.
 */

/**
 * @struct no
 * @brief Estrutura que representa um nó da árvore de Huffman.
 * * Esta estrutura é utilizada tanto de forma isolada na Min-Heap quanto para 
 * interligar os nós e formar a árvore binária de compressão final.
 */
typedef struct no {
    void *byte;            /**<Ponteiro genérico para armazenar o valor do byte (caractere). */
    int frequencia;        /**<Contador de ocorrências do byte no arquivo (critério de prioridade). */
    struct no *esq;        /**<Ponteiro para o filho da esquerda (representa o bit 0). */
    struct no *dir;        /**<Ponteiro para o filho da direita (representa o bit 1). */
} no;

/**
 * @struct heap
 * @brief Estrutura que representa uma Min-Heap (Fila de Prioridade).
 * * Gerencia um array dinâmico de ponteiros para nós, ordenados pela menor frequência.
 */
typedef struct heap {
    no **dados;            /**<Array de ponteiros para os nós da árvore. */
    int tamanho;           /**<Quantidade atual de elementos preenchidos na heap. */
    int capacidade;        /**<Limite máximo de elementos que a heap pode suportar. */
} heap;


/*----------------------------------------------------------------------------*
 * FUNÇÕES DE LEITURA E FREQUÊNCIA                     *
 *----------------------------------------------------------------------------*/

/**
 * @brief Inicializa todas as posições de uma tabela de frequências com zero.
 * @param tab Ponteiro para o array de frequências de tamanho TAM.
 */
void inicializa_tabela_com_zero(unsigned int *tab);

/**
 * @brief Abre um arquivo, lê byte a byte e contabiliza suas respectivas frequências.
 * @param filename Caminho/nome do arquivo a ser lido.
 * @param frequencia Ponteiro para o vetor onde as frequências serão acumuladas.
 */
void readfile(char *filename, unsigned int *frequencia);


/*----------------------------------------------------------------------------*
 * FUNÇÕES DE HEAP E NÓ                            *
 *----------------------------------------------------------------------------*/

/**
 * @brief Aloca memória e inicializa a estrutura de uma Min-Heap.
 * @param capacidade Capacidade máxima de nós que a heap poderá armazenar.
 * @return heap* Ponteiro para a estrutura de heap criada.
 */
heap *criar_heap(int capacidade);

/**
 * @brief Ajusta a propriedade da Min-Heap movendo um elemento para cima (Up-Heap).
 * @param h Ponteiro para a heap.
 * @param i Índice do elemento que será deslocado para cima se for menor que seu pai.
 */
void sobe(heap *h, int i);

/**
 * @brief Insere um novo nó na Min-Heap e reorganiza sua estrutura de prioridade.
 * @param h Ponteiro para a heap.
 * @param novo_no Ponteiro para o nó que será inserido.
 */
void heap_insere(heap *h, no *novo_no);

/**
 * @brief Remove e retorna o nó de menor frequência (raiz) presente na heap.
 * @param h Ponteiro para a heap.
 * @return no* Ponteiro para o nó retirado.
 */
no *heap_retira_min(heap *h);

/**
 * @brief Varre a tabela de frequências e insere os nós dos bytes ativos na Heap.
 * @param h Ponteiro para a heap.
 * @param tab_frequencia Tabela contendo as frequências dos bytes analisados.
 */
void preencher_heap(heap *h, unsigned int *tab_frequencia);

/**
 * @brief Ajusta a propriedade da Min-Heap movendo um elemento para baixo (Down-Heap).
 * @param h Ponteiro para a heap.
 * @param i Índice do elemento inicial que será empurrado para baixo se for maior que seus filhos.
 */
void desce(heap *h, int i);

/**
 * @brief Desaloca a estrutura interna e a própria estrutura da heap da memória.
 * @param h Ponteiro para a heap que será liberada.
 */
void liberar_heap(heap *h);


/*----------------------------------------------------------------------------*
 * FUNÇÕES DE ÁRVORE                              *
 *----------------------------------------------------------------------------*/

/**
 * @brief Combina os nós da heap em pares de menor prioridade até restar apenas um nó raiz.
 * @param h Ponteiro para a heap de nós.
 * @return no* Retorna o ponteiro para a raiz da Árvore de Huffman gerada.
 */
no *montar_arvore(heap *h);

/**
 * @brief Calcula a altura máxima da Árvore de Huffman de forma recursiva.
 * @param raiz Ponteiro para a raiz da árvore.
 * @return int Altura da árvore (maior caminho da raiz até uma folha).
 */
int altura_arvore(no *raiz);

/**
 * @brief Libera recursivamente toda a memória alocada para os nós da árvore (Pós-Ordem).
 * @param raiz Ponteiro para a raiz da árvore a ser destruída.
 */
void liberar_arvore(no *raiz);


/*----------------------------------------------------------------------------*
 * FUNÇÕES DE DICIONÁRIO                            *
 *----------------------------------------------------------------------------*/

/**
 * @brief Aloca dinamicamente uma matriz de strings para mapear os novos códigos dos bytes.
 * @param colunas O tamanho máximo das strings de código (altura da árvore + 1 para o '\\0').
 * @return char** Ponteiro para a matriz de strings alocada.
 */
char **aloca_dicionario(int colunas);

/**
 * @brief Percorre a árvore de Huffman recursivamente e monta os códigos binários de cada caractere folha.
 * @param raiz Ponteiro para o nó corrente da árvore.
 * @param dicionario Matriz onde o código mapeado será guardado na linha correspondente ao byte.
 * @param nivel Profundidade atual na árvore, serve como índice para rastrear a string de caminho.
 * @param caminho Buffer temporário contendo a sequência de '0's e '1's gerada até o momento.
 */
void gerar_dicionario(no *raiz, char **dicionario, int nivel, char *caminho);


/*----------------------------------------------------------------------------*
 * FUNÇÕES DE COMPRESSÃO                           *
 *----------------------------------------------------------------------------*/

/**
 * @brief Calcula o tamanho exato, em bytes, que a árvore de Huffman ocupará no cabeçalho do arquivo.
 * @param raiz Ponteiro para a raiz da árvore.
 * @return int Quantidade total de bytes que a árvore serializada ocupará.
 */
int calcular_tamanho_arvore(no *raiz);

/**
 * @brief Escreve a árvore de Huffman serializada em Pré-Ordem no arquivo binário de saída.
 * @param raiz Ponteiro para a raiz da árvore.
 * @param saida Ponteiro para o arquivo onde a árvore será gravada.
 */
void pre_ordem_arvore(no *raiz, FILE *saida);

/**
 * @brief Calcula o tamanho total que a string de bits combinada ocuparia na memória.
 * @param dicionario Matriz de dicionário contendo os códigos binários mapeados.
 * @param arquivo Arquivo original a ser analisado.
 * @return int O tamanho total calculado da string binária equivalente.
 */
int calcula_tam_string(char **dicionario, FILE *arquivo);

/**
 * @brief Realiza o processo completo de compactação do arquivo.
 * * Gera os metadados (tamanho do lixo e tamanho da árvore), grava o cabeçalho,
 * escreve a árvore em pré-ordem e traduz os bytes originais em bits compactados 
 * aplicando máscaras binárias (bitwise).
 * * @param dicionario Matriz contendo o mapa de códigos binários.
 * @param nome_arquivo String com o nome/caminho do arquivo original.
 * @param arvore Ponteiro para a raiz da Árvore de Huffman.
 * @param tab_frequencia Tabela contendo a contagem de frequência de cada byte.
 */
void compactar(char **dicionario, char *nome_arquivo, no *arvore, unsigned int *tab_frequencia);

#endif /* COMPRESSAO_H */