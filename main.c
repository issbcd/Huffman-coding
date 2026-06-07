#include "compressao.h"
#include "descompressao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    getchar();

    if(opcao == 1)
    {
        printf("Digite o nome do arquivo:\n ");
        scanf("%s", nome_arquivo);
        unsigned int tab_frequencia[TAM];
        inicializa_tabela_com_zero(tab_frequencia);
        readfile(nome_arquivo, tab_frequencia);

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
        
        heap *h = criar_heap(distintos * 2); 
        preencher_heap(h, tab_frequencia); 

        no *arvore = montar_arvore(h);

        int colunas = altura_arvore(arvore) + 1;
        char **dicionario = aloca_dicionario(colunas);
        char *caminho_temporario = (char*) calloc(colunas, sizeof(char));
        gerar_dicionario(arvore, dicionario, 0, caminho_temporario);
        free(caminho_temporario);

        compactar(dicionario, nome_arquivo, arvore, tab_frequencia);

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
        decodificar_arquivo(nome_arquivo);
    }
    else
    {
        printf("Opção inválida.\n");
    }

    return 0;
}
