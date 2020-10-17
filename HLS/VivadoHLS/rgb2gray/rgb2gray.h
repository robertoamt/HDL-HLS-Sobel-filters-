/* Autor: Roberto Millón
 * Universidad Nacional de chilecito
 * mail: rmillon@undec.edu.ar*/


#include <stdio.h>
#include <stdlib.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

//Declaración de tipos

typedef ap_axiu<32,0,0,0> dataU32_t; //Defino tipo de datos de entrada
typedef hls::stream<dataU32_t> streamU32_t; //Defino el stream de datos de entrada

typedef ap_axiu<8,0,0,0> dataU8_t; //Defino tipo de datos de salida
typedef hls::stream<dataU8_t> streamU8_t; //Defino el stream de datos de salida

#define ALTO_MAX 1080  //Alto Max de imagenes que puede procesar
#define ANCHO_MAX 1920  //Ancho Max de imagenes que puede procesar

//Declaración de FUNCIONES

void rgb2gray (streamU32_t&,streamU8_t&,short,short);



