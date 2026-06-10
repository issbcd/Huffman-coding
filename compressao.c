#include "compressao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TAM 256

/*------------------- funcoes --------------------*/

/*-------------------parte 1: funções para leitura e tabela de frequencia--------------------*/
void inicializa_tabela_com_zero(unsigned int *tab) 
{
    for (int i = 0; i < TAM; i++) 
    {
        tab[i] = 0;
    }
}

void readfile(char *filename, unsigned int *frequencia)  //leitura do arquivo
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

/*-------------------parte dois: funções para heap--------------------*/
heap *criar_heap(int capacidade)
{
    heap *nova_heap = (heap*)malloc(sizeof(heap));

    if(nova_heap == NULL) 
    { 
        printf("Erro ao alocar heap.\n"); exit(1); 
    }

    //aloca o array de ponteiros para nós 
    nova_heap->dados = (no **)malloc(sizeof(no *) * capacidade);
    if(nova_heap->dados == NULL)
    {
        printf("Erro ao alocar array da heap.\n");
        exit(1);
    }

    nova_heap->tamanho = 0;// heap começa vazia
    nova_heap->capacidade = capacidade; // guarda o limite máximo            

    return nova_heap;
}

void sobe(heap *h, int i) 
{
    while(i > 0 && h->dados[i]->frequencia < h->dados[(i-1)/2]->frequencia){
        // troca filho com pai
        no *aux = h->dados[i];
        h->dados[i] = h->dados[(i-1)/2];
        h->dados[(i-1)/2] = aux;
        i = (i-1)/2;   // sobe para a posição do pai
    }
}

void heap_insere(heap *h, no *novo_no)
 {
    h->dados[h->tamanho] = novo_no; //insere no final
    sobe(h, h->tamanho); //sobe até a posição certa
    h->tamanho++;
}

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
                printf("Erro de alocação.\n"); exit(1);
            }

            *(unsigned char *)(novo_no->byte) = (unsigned char)i; //armazena o byte
            novo_no->frequencia = tab_frequencia[i];
            novo_no->esq = NULL;
            novo_no->dir = NULL;

            heap_insere(h, novo_no); 
        }
    }
}

/*---------------------parte 3: montar arvore de huffman--------------------*/ 
void desce(heap *h, int i)
{
    int menor = i;// assume que o pai é o menor por enquanto      
    int esq = 2 * i + 1; // índice do filho esquerdo                     
    int dir = 2 * i + 2; //índice do filho direito                      

    /* verifica se o filho esquerdo existe E é menor que o atual "menor"   */
    if(esq < h->tamanho && h->dados[esq]->frequencia < h->dados[menor]->frequencia){
        menor = esq;
    }

    /* verifica se o filho direito existe E é menor que o atual "menor"     */
    if(dir < h->tamanho && h->dados[dir]->frequencia < h->dados[menor]->frequencia){
        menor = dir;
    }

    /* se o menor não é mais o pai, troca e continua descendo               */
    if(menor != i)
    {
        no *aux       = h->dados[i];
        h->dados[i]   = h->dados[menor];
        h->dados[menor] = aux;
        desce(h, menor);    /* chamada recursiva na nova posição            */
    }
}
no *heap_retira_min(heap *h){
    no *minimo  = h->dados[0];         
    h->tamanho--;
    h->dados[0] = h->dados[h->tamanho];

    desce(h, 0);   

    return minimo;
}

//retorna a raiz da árvore
no* montar_arvore(heap *h)
{

    //se tamanho for igual a 1, nao entra no while
    /*chama a funcao remove_inicio_lista e faz oq se faz no while para um unico no*/
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
/*--------------parte quatro - montar dicionario-------------*/
//no vídeo é sugerido que a gente faça uma matriz de strings para o dicionario, tendo como linhas 255(numeros da tabela ascii) e o numero de colunas = colunas da arvore +1
int altura_arvore(no *raiz)
{
    if(raiz == NULL) 
    {
        return -1;
    }
    else
    {
        int esq = altura_arvore(raiz->esq);//chamada recursiva para esquerda
        int dir = altura_arvore(raiz->dir);//chamada recursiva para direita
        //retorna a maior altura entre os filhos, o maior caminho será a altura
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
/*aloca dinamicamente a matriz do dicionario
linhas: TAM
Colunas: altura da arvore + 1 (para o caracter '\0')
*/ 
char **aloca_dicionario(int colunas)
{
    //aloca o vetor de ponteiros(linhas)
    char **dicionario = (char**) malloc(sizeof(char*) * TAM); //TAM representa o número de linhas
    if(dicionario == NULL)
    {
        printf("\nERRO DE ALOCAÇÃO DE MEMÓRIA.\n");
        exit(1);
    }
    //aloca cada linha individualmente usando dicionario[i]
    for(int i = 0; i < TAM; i++)
    {
       dicionario[i] = (char*)calloc(colunas, sizeof(char));//usamos calloc pois ele aloca a memoria e limpa para 0
       
       if(dicionario[i] == NULL)
       {
           printf("\nERRO DE ALOCAÇÃO DE MEMÓRIA.\n");
           exit(1);
       }    
    }
    return dicionario;
}
/*
percorre a arvore recursivamente preenchendo o dicionario
parametros
nivel: controla a posicao(coluna) atual onde vamos inserir '0' ou '1'
caminho: armazena a sequência de 0s e 1s
*/
void gerar_dicionario(no *raiz, char **dicionario, int nivel, char *caminho)
{
    if(raiz == NULL)
    {
        return;
    }
    if(raiz->esq == NULL && raiz->dir == NULL) //é uma folha
    {
        caminho[nivel] = '\0';
        unsigned char caractere = *(unsigned char*)(raiz->byte);
        strcpy(dicionario[caractere], caminho); //salva o codigo gerado na linha desse caractere dentro do dicionario
    }
    else //não é folha, é um no interno
    {
        caminho[nivel] = '0'; //caminho para a esquerda: adiciona '0' na posição atual e avança o nivel para o próximo filho
        gerar_dicionario(raiz->esq, dicionario, nivel + 1, caminho);

        caminho[nivel] = '1';
        gerar_dicionario(raiz->dir, dicionario, nivel + 1, caminho);    
    }
   
}
/*para fazer o cabeçalho:*/
int calcular_tamanho_arvore(no *raiz){
    if(raiz == NULL){
        return 0;  
    }

    //se for uma folha
    if(raiz->esq == NULL && raiz->dir == NULL){
        unsigned char c = *(unsigned char*)(raiz->byte);
        
        if(c == '*' || c == '\\'){
            return 2;
        }
        return 1;
    }
    return 1 + calcular_tamanho_arvore(raiz->esq) + calcular_tamanho_arvore(raiz->dir);   //retorna 1 + o tamanho da arvore esquerda + o tamanho da arvore direita
   
}

void pre_ordem_arvore(no *raiz, FILE *saida){
    if(raiz == NULL){
        return; 
    }
    
    //se for uma folha
    if(raiz->esq == NULL && raiz->dir == NULL){
        unsigned char c = *(unsigned char*)(raiz->byte);
        
        if(c == '*' || c == '\\')
        {
            fputc('\\', saida);
        }
        
        fputc(c, saida);
    }
    //se for nó interno
    else{
        fputc('*', saida);
        pre_ordem_arvore(raiz->esq, saida);
        pre_ordem_arvore(raiz->dir, saida);
        
    }
    
}
/*-----------------parte cinco- codificar(comprimir)---------------*/

/*funcao que recebe dicionario e caminho do arquivo, abre o arquivo e aloca o espaço necessario na memoria e monta a string com os codigos concatenados*/

void compactar(char **dicionario, char *nome_arquivo, no *arvore, unsigned int *tab_frequencia)
{
    //abre o arquivo original para a leitura
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo.\n");
        exit(1);
    }

    //nome da saida para .huff
    char nome_saida[256];
    strncpy(nome_saida, nome_arquivo, sizeof(nome_saida) - 1);
    nome_saida[sizeof(nome_saida) - 1] = '\0';

    //pegar o ponto anterior a .huff
    char *ponto = strrchr(nome_saida, '.'); //funcao strrchr: procura a primeira aparicao de algum caractere determinado na string
    if (ponto != NULL) 
    {
        *ponto = '\0'; //ou seja, não há, não precisaremos tirar
    }
    //função strncat: o tipo original do nosso arquivo será descartado pois limita o tamanho do nome 
    strncat(nome_saida, ".huff", sizeof(nome_saida) - strlen(nome_saida) - 1);

    //abre o arquivo de saida em escrita binaria
    FILE *saida = fopen(nome_saida, "wb");
    if(saida == NULL)
    {
        printf("Erro ao abrir arquivo compactado.\n");
        fclose(arquivo);
        exit(1);
    }
     
    //cabeçalho: calcular total de bits
    unsigned long long total_bits = 0;
    for(int i = 0; i < TAM; i++)
    {
        if(tab_frequencia[i] > 0){
            total_bits += (unsigned long long)tab_frequencia[i] * (strlen(dicionario[i]));
        }
    }
    //calcular lixo
    int lixo;
    if(total_bits % 8 == 0)
    {
        lixo = 0;
    }
    else
    {
        lixo = 8 - (total_bits % 8);//quantos bits sobram para fechar o ultimp byte
    }

    int tamanho_arvore = calcular_tamanho_arvore(arvore);
    /*
    montar dois bytes do cabeçalhp
    byte1: lixo
    byte2: arvore
    */
    unsigned char byte1 = (lixo << 5) | (tamanho_arvore >> 8); //funcao ou em bits
    unsigned char byte2 = tamanho_arvore & 0xFF; //pega os ultimos 8 bits //funcao e em bits

    fputc(byte1, saida);
    fputc(byte2, saida);

    pre_ordem_arvore(arvore, saida);
    
    int bytelido;
    unsigned char byte_buffer = 0;//acumulador em que vmos montando os 8 bits
    int contador_bits = 0; //controla quantos bits já foram acumulados em byte_buffer

    rewind(arquivo);
    //le o arquivo original caracter por caracter
    while((bytelido = fgetc(arquivo))!=EOF){
        char *codigo = dicionario[bytelido]; //acessa a string de '0' e '1' no dicionario para esse byte

        //percorre cada bit da string do codigo do dicionario
        for(int i = 0; codigo[i]!= '\0'; i++){
           if(codigo[i] == '1'){
                byte_buffer = byte_buffer | (1 << (7 - contador_bits)); //se o caracter for '1', liga o bit na posicao correta do buffer usando bitwise
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


// Libera recursivamente toda a memória ocupada pela árvore de Huffman
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
void liberar_heap(heap *h)
{
    free(h->dados);  //libera o array de ponteiros                         
    free(h);         //libera a struct heap em si                          
}

