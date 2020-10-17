/* Autor: Roberto Millón
 * Universidad Nacional de chilecito
 * mail: rmillon@undec.edu.ar
 *
 * La función u8tou32 recibe 1 byte y lo asigna a los tres canales del formato RGB (triplica los bytes)
 * para trabajar con el DMA
 *
 * */


#include "u8tou32.h"
#include "bmpTools.h"
#include <hls_opencv.h>

int main ()
{
	pix_t *pix_src,*pix_dst;
	unsigned int *uintPtr;
	cabecera_t cabecera;
	dataU8_t pixel_out;
	streamU8_t strm_src ("Stream Fuente"); //Stream Fuente para enviar la imagen en escala de grises
	streamU32_t strm_dst ("Stream Destino");//Stream Destino que recibe la imagen en escala de grises

	char imageNamesSrc[][40] = {
							"Test_Images/4-2-03-GRAY.bmp",
							"Test_Images/4-2-07-GRAY.bmp",
							"Test_Images/graf1-GRAY.bmp",
							"Test_Images/kodim23-GRAY.bmp"
							};
	char imageNamesDst[][40] = {
							"4-2-03-SAME.bmp",
							"4-2-07-SAME.bmp",
							"graf1-SAME.bmp",
							"kodim23-SAME.bmp"
							   };


	for (char k=0;k<sizeof(imageNamesDst)/sizeof(imageNamesDst[0]);++k)
	{
	printf("************Iniciando envío de imagen %d********\n",k+1);

	pix_src = loadBmp(*(imageNamesSrc+k),&cabecera);//Almaceno los pixeles en el puntero pix_src
	uintPtr = (unsigned int *)pix_src; //Asigno imagen fuente a un puntero de enteros (para enviar bloques de 4 bytes)



	for(int i=0; i<cabecera.height; ++i) //Leo toda la imagen pixel a pixel
		for(int j=0; j<cabecera.width; ++j)
		{
			pixel_out.data=pix_src[i*cabecera.width + j].b;
			pixel_out.strb=1;
			j==(cabecera.width-1)? pixel_out.last=1 : pixel_out.last=0;
			strm_src.write(pixel_out);
		}

	printImageAttrib(&cabecera);//Imprimir info de la imagen. Comentar si no se requiere

	u8tou32(strm_src,strm_dst,cabecera.height,cabecera.width); //Convertir RGB a GRAY

	pix_dst = (pix_t*) malloc(cabecera.size);//Pido memoria para el puntero de la imagen destino

	uintPtr = (unsigned int *)pix_dst; //Asigno imagen fuente a un puntero de enteros (para enviar bloques de 4 bytes)

	short col_u32 = (cabecera.width*3)/4;//Número de columnas considerando 4 bytes

	for(int i=0; i<cabecera.height; ++i) //Recibo los pixeles procesados
			for(int j=0; j<col_u32; ++j)
				uintPtr[i*col_u32 + j] = strm_dst.read().data;

	saveBmp(*(imageNamesDst+k),&cabecera,pix_dst);//Guardo los pixeles procesados en una archivo

	free (pix_src);//Libero la memoria asignada al puntero pix_src
	free (pix_dst);//Libero la memoria asignada al puntero pix_dst

	printf("************proceso exitoso imagen %d********\n",k+1);
	}


	return 0;
}



