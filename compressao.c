#include "compressao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 256

/**
 * Inicializa todas as posições da tabela de frequência com o valor zero.
 * * @param tab Ponteiro para o array (tabela) de frequências de tamanho TAM.
 */
void inicializa_tabela_com_zero(unsigned int *tab) 
{
    for (int i = 0; i < TAM; i++) 
    {
        tab[i] = 0;
    }
}

/**
 * Lê um arquivo binário e contabiliza a frequência de ocorrência de cada byte.
 * * @param filename O nome ou caminho do arquivo a ser lido.
 * @param frequencia Ponteiro para a tabela onde as frequências serão acumuladas.
 */
void readfile(char *filename, unsigned int *frequencia)
{
    FILE *arquivo = fopen(filename, "rb"); 
    
    if(arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo: %s\n", filename);
        exit(1);
    }

    int bytelido;
    while((bytelido = fgetc(arquivo)) != EOF){
        frequencia[bytelido]++;
    }
    
    fclose(arquivo);
}


/**
 * Aloca e inicializa uma estrutura de Min-Heap com uma capacidade definida.
 * * @param capacidade O número máximo de elementos que a heap pode suportar.
 * @return heap* Ponteiro para a estrutura da heap criada.
 */
heap *criar_heap(int capacidade)
{
    heap *nova_heap = (heap*)malloc(sizeof(heap));

    if(nova_heap == NULL) 
    { 
        printf("Erro ao alocar heap.\n"); exit(1); 
    }

    nova_heap->dados = (no **)malloc(sizeof(no *) * capacidade);
    if(nova_heap->dados == NULL)
    {
        printf("Erro ao alocar array da heap.\n");
        exit(1);
    }

    nova_heap->tamanho = 0;
    nova_heap->capacidade = capacidade;            

    return nova_heap;
}

/**
 * Desloca um elemento para cima (Up-Heap) para manter a propriedade da Min-Heap.
 * Utilizado logo após a inserção de um novo nó.
 * * @param h Ponteiro para a heap.
 * @param i Índice do elemento que precisa subir.
 */
void sobe(heap *h, int i) 
{
    while(i > 0 && h->dados[i]->frequencia < h->dados[(i-1)/2]->frequencia){
        no *aux = h->dados[i];
        h->dados[i] = h->dados[(i-1)/2];
        h->dados[(i-1)/2] = aux;
        i = (i-1)/2;
    }
}

/**
 * Insere um novo nó na Min-Heap e reorganiza sua estrutura.
 * * @param h Ponteiro para a heap.
 * @param novo_no Ponteiro para o nó da árvore de Huffman a ser inserido.
 */
void heap_insere(heap *h, no *novo_no)
{
    h->dados[h->tamanho] = novo_no;
    sobe(h, h->tamanho);
    h->tamanho++;
}

/**
 * Varre a tabela de frequências e insere todos os caracteres que apareceram 
 * no arquivo (frequência > 0) dentro da Min-Heap.
 * * @param h Ponteiro para a heap.
 * @param tab_frequencia Ponteiro para a tabela de frequências populada.
 */
void preencher_heap(heap *h, unsigned int *tab_frequencia)
{
    for(int i = 0; i < TAM; i++){
        if(tab_frequencia[i] > 0) 
        {
            no *novo_no = (no *)malloc(sizeof(no));
        
            if(novo_no == NULL) 
            {
                printf("Erro de alocação.\n");
                exit(1); 
            }

            novo_no->byte = malloc(sizeof(unsigned char));
            if(novo_no->byte == NULL)
            {
                printf("Erro de alocação.\n");
                exit(1);
            }

            *(unsigned char *)(novo_no->byte) = (unsigned char)i;
            novo_no->frequencia = tab_frequencia[i];
            novo_no->esq = NULL;
            novo_no->dir = NULL;

            heap_insere(h, novo_no); 
        }
    }
}


/**
 * Desloca um elemento para baixo (Down-Heap) para manter a propriedade da Min-Heap.
 * Utilizado logo após a remoção do elemento mínimo (raiz da heap).
 * * @param h Ponteiro para a heap.
 * @param i Índice do elemento que precisa descer.
 */
void desce(heap *h, int i)
{
    int menor = i;      
    int esq = 2 * i + 1;                     
    int dir = 2 * i + 2;                      

    if(esq < h->tamanho && h->dados[esq]->frequencia < h->dados[menor]->frequencia){
        menor = esq;
    }

    if(dir < h->tamanho && h->dados[dir]->frequencia < h->dados[menor]->frequencia){
        menor = dir;
    }

    if(menor != i)
    {
        no *aux       = h->dados[i];
        h->dados[i]   = h->dados[menor];
        h->dados[menor] = aux;
        desce(h, menor);
    }
}

/**
 * Remove e retorna o nó com a menor frequência presente na Min-Heap.
 * * @param h Ponteiro para a heap.
 * @return no* Ponteiro para o nó de menor frequência retirado.
 */
no *heap_retira_min(heap *h){
    no *minimo  = h->dados[0];         
    h->tamanho--;
    h->dados[0] = h->dados[h->tamanho];

    desce(h, 0);   

    return minimo;
}

/**
 * Combina os nós da Min-Heap para construir a Árvore de Huffman.
 * * @param h Ponteiro para a heap populada.
 * @return no* Ponteiro para o nó raiz da Árvore de Huffman finalizada.
 */
no* montar_arvore(heap *h)
{
    if(h->tamanho == 1)
    {
        return heap_retira_min(h);
    }

    while(h->tamanho > 1){
        no *primeiro = heap_retira_min(h);
        no *segundo = heap_retira_min(h);

        no *pai = (no*)malloc(sizeof(no));

        if(pai == NULL)
        {
            printf("\nERRO DE ALOCAÇÃO DE MEMÓRIA.\n");
            exit(1);
        }

        pai->byte = malloc(sizeof(unsigned char));
        if(pai->byte == NULL) {
            printf("\nERRO DE ALOCAÇÃO DE MEMÓRIA PARA O BYTE PAI.\n");
            exit(1);
        }
        *(unsigned char*)(pai->byte) = '*';

        pai->frequencia = primeiro->frequencia + segundo->frequencia; 
        pai->esq = primeiro; 
        pai->dir = segundo;  
        
        heap_insere(h, pai); 
    }
    return heap_retira_min(h);
}


/**
 * Calcula a altura da Árvore de Huffman recursivamente.
 * * @param raiz Ponteiro para a raiz da árvore (ou subárvore).
 * @return int A altura da árvore. Retorna -1 se a árvore estiver vazia.
 */
int altura_arvore(no *raiz)
{
    if(raiz == NULL) 
    {
        return -1;
    }
    else
    {
        int esq = altura_arvore(raiz->esq);
        int dir = altura_arvore(raiz->dir);
        
        if(esq > dir)
        {
            return esq + 1;
        }
        else
        {
            return dir + 1;
        }
    }
}

/**
 * Aloca dinamicamente uma matriz de caracteres (strings) para o dicionário.
 * As linhas correspondem aos 256 possíveis caracteres ASCII/Bytes.
 * * @param colunas O número de colunas (altura da árvore + 1 para o caractere '\0').
 * @return char** Ponteiro para a matriz de strings alocada.
 */
char **aloca_dicionario(int colunas)
{
    char **dicionario = (char**) malloc(sizeof(char*) * TAM); 
    if(dicionario == NULL)
    {
        printf("\nERRO DE ALOCAÇÃO DE MEMÓRIA.\n");
        exit(1);
    }
    for(int i = 0; i < TAM; i++)
    {
       dicionario[i] = (char*)calloc(colunas, sizeof(char));
       
       if(dicionario[i] == NULL)
       {
           printf("\nERRO DE ALOCAÇÃO DE MEMÓRIA.\n");
           exit(1);
       }    
    }
    return dicionario;
}

/**
 * Percorre a árvore de Huffman de forma recursiva, gerando e armazenando 
 * os novos códigos binários de cada caractere na tabela hash/matriz do dicionário.
 * * @param raiz Ponteiro para o nó atual da árvore.
 * @param dicionario Matriz onde serão armazenadas as strings binárias resultantes.
 * @param nivel Controla a coluna/posição atual da string 'caminho'.
 * @param caminho Buffer de caracteres que armazena a sequência temporária de '0's e '1's.
 */
void gerar_dicionario(no *raiz, char **dicionario, int nivel, char *caminho)
{
    if(raiz == NULL)
    {
        return;
    }
    if(raiz->esq == NULL && raiz->dir == NULL) 
    {
        caminho[nivel] = '\0';
        unsigned char caractere = *(unsigned char*)(raiz->byte);
        strcpy(dicionario[caractere], caminho); 
    }
    else 
    {
        caminho[nivel] = '0'; 
        gerar_dicionario(raiz->esq, dicionario, nivel + 1, caminho);

        caminho[nivel] = '1';
        gerar_dicionario(raiz->dir, dicionario, nivel + 1, caminho);    
    }
}

/**
 * Calcula o tamanho (em bytes) que a árvore ocupará no cabeçalho do arquivo compactado.
 * Caracteres especiais de escape ('*' e '\') contam em dobro.
 * * @param raiz Ponteiro para a raiz da árvore de Huffman.
 * @return int O tamanho total em bytes necessário para mapear a árvore.
 */
int calcular_tamanho_arvore(no *raiz){
    if(raiz == NULL){
        return 0;  
    }

    if(raiz->esq == NULL && raiz->dir == NULL){
        unsigned char c = *(unsigned char*)(raiz->byte);
        
        if(c == '*' || c == '\\'){
            return 2;
        }
        return 1;
    }
    return 1 + calcular_tamanho_arvore(raiz->esq) + calcular_tamanho_arvore(raiz->dir);
}

/**
 * Escreve a estrutura da Árvore de Huffman no arquivo de saída utilizando o percurso em pré-ordem.
 * * @param raiz Ponteiro para a raiz da árvore.
 * @param saida Ponteiro do arquivo compactado (.huff) onde a árvore será gravada.
 */
void pre_ordem_arvore(no *raiz, FILE *saida){
    if(raiz == NULL){
        return; 
    }
    
    if(raiz->esq == NULL && raiz->dir == NULL){
        unsigned char c = *(unsigned char*)(raiz->byte);
        
        if(c == '*' || c == '\\')
        {
            fputc('\\', saida);
        }
        fputc(c, saida);
    }
    else{
        fputc('*', saida);
        pre_ordem_arvore(raiz->esq, saida);
        pre_ordem_arvore(raiz->dir, saida);
    }
}


/**
 * Executa todo o processo de compactação de um arquivo: calcula os bits necessários, 
 * define o tamanho do "lixo", grava os dois bytes de cabeçalho, exporta a árvore em pré-ordem
 * e realiza a compressão bit a bit através de operações bitwise.
 * * @param dicionario Matriz contendo as strings binárias mapeadas dos caracteres.
 * @param nome_arquivo String contendo o nome/caminho do arquivo original.
 * @param arvore Ponteiro para a raiz da árvore de Huffman.
 * @param tab_frequencia Array contendo as frequências originais dos bytes.
 */
void compactar(char **dicionario, char *nome_arquivo, no *arvore, unsigned int *tab_frequencia)
{
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo.\n");
        exit(1);
    }

    char nome_saida[256];
    strncpy(nome_saida, nome_arquivo, sizeof(nome_saida) - 1);
    nome_saida[sizeof(nome_saida) - 1] = '\0';

    char *ponto = strrchr(nome_saida, '.'); 
    if (ponto != NULL) 
    {
        *ponto = '\0'; 
    }
    strncat(nome_saida, ".huff", sizeof(nome_saida) - strlen(nome_saida) - 1);

    FILE *saida = fopen(nome_saida, "wb");
    if(saida == NULL)
    {
        printf("Erro ao abrir arquivo compactado.\n");
        fclose(arquivo);
        exit(1);
    }
     
    unsigned long long total_bits = 0;
    for(int i = 0; i < TAM; i++)
    {
        if(tab_frequencia[i] > 0){
            total_bits += (unsigned long long)tab_frequencia[i] * (strlen(dicionario[i]));
        }
    }

    int lixo;
    if(total_bits % 8 == 0)
    {
        lixo = 0;
    }
    else
    {
        lixo = 8 - (total_bits % 8);
    }

    int tamanho_arvore = calcular_tamanho_arvore(arvore);

    unsigned char byte1 = (lixo << 5) | (tamanho_arvore >> 8); 
    unsigned char byte2 = tamanho_arvore & 255; 

    fputc(byte1, saida);
    fputc(byte2, saida);

    pre_ordem_arvore(arvore, saida);
    
    int bytelido;
    unsigned char byte_buffer = 0;
    int contador_bits = 0; 

    rewind(arquivo);
    while((bytelido = fgetc(arquivo)) != EOF){
        char *codigo = dicionario[bytelido]; 

        for(int i = 0; codigo[i] != '\0'; i++){
           if(codigo[i] == '1'){
                byte_buffer = byte_buffer | (1 << (7 - contador_bits)); 
           }
           contador_bits++;
           if(contador_bits == 8){
            fputc(byte_buffer, saida);
            byte_buffer = 0;
            contador_bits = 0;
           }
        }
    }   
    
    if(contador_bits > 0){
        fputc(byte_buffer, saida);
    }
    
    printf("arquivo compactado, arquivo gerado: %s\n", nome_saida);
    fclose(saida);
    fclose(arquivo);
}

/**
 * Desaloca recursivamente toda a memória ocupada pela árvore de Huffman 
 * para prevenir vazamentos de memória (memory leaks).
 * * @param raiz Ponteiro para a raiz da árvore (ou subárvore) que será limpa.
 */
void liberar_arvore(no *raiz)
{
    if (raiz == NULL) return;
    liberar_arvore(raiz->esq);
    liberar_arvore(raiz->dir);
    if (raiz->byte != NULL) {
        free(raiz->byte);
    }
    free(raiz);
}

/**
 * Libera a memória da estrutura da heap e de seu array interno de dados.
 ** @param h Ponteiro para a heap que será limpa.
 */
void liberar_heap(heap *h)
{
    free(h->dados);                           
    free(h);                                   
}