<div align="center">
  <br>
  <strong>UNIVERSIDADE FEDERAL DE ALAGOAS - UFAL</strong><br>
  <strong>INSTITUTO DE COMPUTAÇÃO - IC</strong><br>
  Disciplina: Estrutura de Dados (ED)<br>
  Professor: Dr. Márcio Ribeiro
</div>

<hr>

# Projeto Final: Compressor de Arquivos com Algoritmo de Huffman em C

## Resumo do Projeto

Este repositório contém a implementação de um compressor e descompressor de arquivos desenvolvido na linguagem de programação C, baseado no **Algoritmo de Huffman**. O sistema processa qualquer arquivo binário, construindo uma árvore de frequências para gerar códigos de comprimento variável, reduzindo o tamanho do arquivo original ao substituir bytes mais frequentes por representações de menor quantidade de bits.

O principal objetivo técnico deste projeto foi a aplicação prática de estruturas de dados dinâmicas — heap de mínimo e árvore binária — para a construção eficiente do código de Huffman e a manipulação de arquivos em nível de bits.

## Detalhes da Implementação

A solução proposta destaca-se pelo uso exclusivo de alocação dinâmica de memória e operações *bitwise* para a manipulação precisa dos dados. As principais implementações incluem:

1. **Min-Heap Dinâmica:** Utilização de um **heap de mínimo** baseado em array de ponteiros para nós, com as operações `sobe` e `desce` garantindo a propriedade de heap a cada inserção e remoção. A heap é construída a partir da tabela de frequências dos bytes do arquivo original e serve de base para a montagem da árvore de Huffman.

2. **Árvore de Huffman e Dicionário:** Construção de uma **Árvore Binária** a partir da heap, unindo iterativamente os dois nós de menor frequência. A partir da árvore resultante, um dicionário de strings de `'0'` e `'1'` é gerado para cada símbolo, onde o comprimento de cada código é proporcional ao inverso da frequência do byte.

3. **Compressão com Cabeçalho Compacto:** O arquivo comprimido (extensão `.huff`) armazena um cabeçalho de apenas **2 bytes** codificado via operações *bitwise*, contendo o tamanho do lixo (3 bits) e o tamanho da árvore serializada (13 bits), seguido da árvore em pré-ordem e dos dados comprimidos.

4. **Descompressão por Navegação na Árvore:** A descompressão lê o cabeçalho, reconstrói a árvore a partir da serialização em pré-ordem e percorre a árvore bit a bit para recuperar os bytes originais, descartando os bits de lixo ao processar o último byte.

## Estrutura do Código Fonte

* `compressao.h` / `compressao.c`: Definição e implementação das estruturas de dados (`no`, `heap`), da min-heap, da árvore de Huffman, do dicionário e da lógica de compressão.
* `descompressao.h` / `descompressao.c`: Implementação da reconstrução da árvore a partir do arquivo comprimido e da decodificação dos bits para recuperação do arquivo original.
* `main.c`: Ponto de entrada do sistema, responsável por ler a opção do usuário, orquestrar os módulos de compressão e descompressão e liberar toda a memória alocada.

## Compilação e Execução

Para a compilação do projeto, recomenda-se a utilização de um compilador C padrão (como o GCC). O processo pode ser realizado através do terminal:

```bash
gcc main.c compressao.c descompressao.c -o huffman
```

Em seguida, execute o binário gerado:

```bash
./huffman
```

O programa solicitará a opção desejada:

```
Opção 1: Compactar arquivo
Opção 2: Descompactar arquivo
```

Para compactar, informe o nome do arquivo original (ex: `texto.txt`). O arquivo comprimido será gerado com a extensão `.huff` (ex: `texto.huff`). Para descompactar, informe o arquivo `.huff` e o arquivo original será recuperado.

## Equipe Desenvolvedora

```Este projeto foi realizado pelas seguintes discentes:
Ana Carolina Cavalcante de Jesus

Maria Luisa Silva Nunes de Souza

Sophia Byernes Carvalho Duarte

