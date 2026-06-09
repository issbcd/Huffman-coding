#ifndef DESCOMPRESSAO_H
#define DESCOMPRESSAO_H

#include "compressao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


no *reconstruir_arvore(FILE *entrada, int *bytes_arvore);/*le o arquivo comprimido p poder reconstruir a arvore do huffman e conta quantos bytes da arvore ja foram lidos*/
void decodificar_arquivo(char *arquivo_huff); /*funcao principal, abre o arquivo, reconstrói a arvore e decodifica os bits*/

#endif
