/* Autor: Roberto Millón
 * Universidad Nacional de chilecito
 * mail: rmillon@undec.edu.ar
 */


#ifndef SOBEL_H_
#define SOBEL_H_

#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <hls_math.h>

//Declaración de MACROS


#define WIN_SIZE 3 		//Tamaño de la ventana (3x3)
#define LOOP_OFFSET (WIN_SIZE-1)/2
#define ALTO_MAX (1080+LOOP_OFFSET)  //Alto de la imagen 514
#define ANCHO_MAX (1920+LOOP_OFFSET)  //Ancho de la imagen
#define ABS(x) ((x>0)? x : -x) //Macro para obtener el valor absoluto
//#define RMS(a, b) (sqrt ((a) * (a) + (b) * (b)))//Macro para obtener la raiz cuadrada

//Declaración de tipos

typedef ap_axiu<8,0,0,0> pixel_t; //Defino tipo de pixel para el Stream with side channels (8 bits -> Imagen escala de grises)
typedef hls::stream<pixel_t> stream_t; //Defino el stream de pixeles


//Declaración de FUNCIONES

//pixel_t send_pixel (unsigned char,int);
unsigned char convolut(unsigned char[][WIN_SIZE]);
void edge_detect (stream_t&,stream_t&,int,int);


#endif
