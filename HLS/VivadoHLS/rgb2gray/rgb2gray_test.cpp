/* Autor: Roberto Millón
 * Universidad Nacional de chilecito
 * mail: rmillon@undec.edu.ar*/


#include "rgb2gray.h"
#include "bmpTools.h"
#include <hls_opencv.h>

int main ()
{
	pix_t *pix_src,*pix_dst;
	unsigned int *uintPtr;
	cabecera_t cabecera;
	dataU32_t pixel_out;
	streamU32_t strm_src ("Stream Fuente"); //Stream Fuente para enviar la imagen a color RGB
	streamU8_t strm_dst ("Stream Destino");//Stream Destino que recibe la imagen en escala de grises GRAY

	char imageNamesSrc[][40] = {
							"Test_Images/4-2-03.bmp",
							"Test_Images/4-2-07.bmp",
							"Test_Images/graf1.bmp",
							"Test_Images/kodim23.bmp"
							};
	char imageNamesDst[][40] = {
							"4-2-03-GRAY.bmp",
							"4-2-07-GRAY.bmp",
							"graf1-GRAY.bmp",
							"kodim23-GRAY.bmp"
							   };


	for (char k=0;k<sizeof(imageNamesDst)/sizeof(imageNamesDst[0]);++k)
	{
	printf("************Iniciando conversión RGB a GRAY en imagen %d********\n",k+1);

	pix_src = loadBmp(*(imageNamesSrc+k),&cabecera);//Almaceno los pixeles en el puntero pix_src
	uintPtr = (unsigned int *)pix_src; //Asigno imagen fuente a un puntero de enteros (para enviar bloques de 4 bytes)

	short col_u32 = (cabecera.width*3)/4;//Número de columnas considerando 4 bytes

	for(int i=0; i<cabecera.height; ++i) //Leo toda la imagen pixel a pixel
		for(int j=0; j<col_u32; ++j)
		{
			pixel_out.data=uintPtr[i*col_u32 + j];
			pixel_out.strb=1;
			if(j==col_u32-1)
				pixel_out.last=1;
			else
				pixel_out.last=0;
			strm_src.write(pixel_out);
		}

	printImageAttrib(&cabecera);//Imprimir info de la imagen. Comentar si no se requiere

	rgb2gray(strm_src,strm_dst,cabecera.height,cabecera.width); //Convertir RGB a GRAY

	pix_dst = (pix_t*) malloc(cabecera.size);//Pido memoria para el puntero de la imagen destino

	for(int i=0; i<cabecera.height; ++i) //Recibo los pixeles procesados
			for(int j=0; j<cabecera.width; ++j)
				pix_dst[i*cabecera.width + j].r=pix_dst[i*cabecera.width + j].b=pix_dst[i*cabecera.width + j].g= strm_dst.read().data;

	saveBmp(*(imageNamesDst+k),&cabecera,pix_dst);//Guardo los pixeles procesados en una archivo

	free (pix_src);//Libero la memoria asignada al puntero pix_src
	free (pix_dst);//Libero la memoria asignada al puntero pix_dst

	printf("************proceso exitoso imagen %d********\n",k+1);
	}


	return 0;
}



