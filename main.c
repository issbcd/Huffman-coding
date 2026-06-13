#include "compressao.h"
#include "descompressao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @file main.c
 * @brief Ponto de entrada do programa de compressão e descompressão de arquivos usando o algoritmo de Huffman.
 */

/**
 * Interface de usuário via terminal. Permite ao usuário escolher entre compactar ou 
 * descompactar um arquivo, gerenciando o fluxo de leitura, construção das estruturas 
 * (tabela de frequência, heap, árvore e dicionário) e a liberação de memória.
 * * @return int Retorna 0 em caso de execução bem-sucedida ou 1 se houver erro de entrada.
 */
int main()
{
    int opcao;
    char nome_arquivo[256];

    printf("Opção 1: Compactar arquivo\n");
    printf("Opção 2: Descompactar arquivo\n");
    printf("Digite sua opção: ");

    if(scanf("%d", &opcao) != 1)
    {
        printf("Entrada invalida.\n");
        return 1;
    }
    getchar(); // Limpa o caractere de nova linha '\n' do buffer

    if(opcao == 1)
    {
        printf("Digite o nome do arquivo:\n ");
        scanf("%s", nome_arquivo);
        
        unsigned int tab_frequencia[TAM];
        inicializa_tabela_com_zero(tab_frequencia);
        readfile(nome_arquivo, tab_frequencia);

        // Conta quantos bytes diferentes aparecem no arquivo original
        int distintos = 0;
        for(int i = 0; i < TAM; i++)
        {
            if(tab_frequencia[i] > 0)
            {
                distintos++;
            }
        }
        
        if(distintos == 0)
        {
            printf("Arquivo vazio! Nao ha dados para compactar.\n");
            return 0; 
        }
        
        // Inicializa a fila de prioridade (Min-Heap)
        heap *h = criar_heap(distintos * 2); 
        preencher_heap(h, tab_frequencia); 

        // Constrói a Árvore binária de Huffman
        no *arvore = montar_arvore(h);

        // Aloca e gera o Dicionário de caminhos binários
        int colunas = altura_arvore(arvore) + 1;
        char **dicionario = aloca_dicionario(colunas);
        char *caminho_temporario = (char*) calloc(colunas, sizeof(char));
        gerar_dicionario(arvore, dicionario, 0, caminho_temporario);
        free(caminho_temporario);

        /* Caso especial: Trata arquivos compostos por um único caractere repetido.
         * Se a árvore possuir apenas o nó raiz (sem filhos), define o código "0" 
         * manualmente para esse caractere no dicionário. */
        if (arvore->esq == NULL && arvore->dir == NULL) 
        {
            unsigned char c = *(unsigned char *)(arvore->byte);
            strcpy(dicionario[c], "0");
        }

        // Executa a codificação bitwise e gera o arquivo .huff
        compactar(dicionario, nome_arquivo, arvore, tab_frequencia);

        // Liberação de toda a memória dinâmica alocada no processo
        liberar_heap(h);
        liberar_arvore(arvore);
        for(int i = 0; i < TAM; i++){
            free(dicionario[i]);
        }       
        free(dicionario);
    }
    else if(opcao == 2)
    {
        printf("Digite o nome do arquivo:\n ");
        scanf("%s", nome_arquivo);
        
        // Executa o processo inverso de decodificação
        decodificar_arquivo(nome_arquivo);
    }
    else
    {
        printf("Opção inválida.\n");
    }

    return 0;
}