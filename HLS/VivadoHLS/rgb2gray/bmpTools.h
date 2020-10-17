/* Autores: Emmanuel Frat-Roberto Millón
 * Universidad Nacional de chilecito
 * mail: fefrati@undec.edu.ar-rmillon@undec.edu.ar*/



#ifndef BMPTOOLS_H
#define BMPTOOLS_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte_t;
typedef unsigned short byte2_t;
typedef unsigned int byte4_t;
typedef struct {unsigned char r, g, b;} pix_t;


typedef struct __attribute__ ((packed))
{
  byte_t signature[2];

  byte4_t size ;

  byte2_t reserved1 ;
  byte2_t reserved2 ;

  byte4_t offset ;
  byte4_t headerSize ;
  byte4_t width ;
  byte4_t height ;

  byte2_t planes ;
  byte2_t bitsPerPixel ;

  byte4_t compression ;
  byte4_t imageSize ;
  byte4_t horizontal ;
  byte4_t vertical ;
  byte4_t colors ;
  byte4_t importantColors ;
} cabecera_t;

pix_t *loadBmp(char *, cabecera_t *);
void printImageAttrib(cabecera_t *);
void saveBmp(char*,cabecera_t*,pix_t*);


#endif

