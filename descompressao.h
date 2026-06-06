#ifndef descompressao.h
#define descompressao.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

no *reconstruir_arv(FILE *entrada, int *ja_lidos);/*le o arquivo comprimido p poder reconstruir a arvore do huffman e conta quantos bytes da arvore ja foram lidos*/
void descompactacao (const char *nome_entrada, const char *nome_saida);/*le o arquivo .huff, reconstroi a arvore e decodifica os bits gravando o arquivo original*/

#endif
