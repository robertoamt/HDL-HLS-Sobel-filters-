/* Autor: Roberto Millón
 * Universidad Nacional de chilecito
 * mail: rmillon@undec.edu.ar
 */


#include "sobel_ABS.h"
#include "bmpTools.h"
#include <hls_opencv.h>



int main ()
{
	pix_t *pix_src;
	cabecera_t cabecera;
	pixel_t pixel_out;
	stream_t strm_src ("Stream Fuente"); //Stream Fuente para enviar la imagen a la f(x)
	stream_t strm_dst ("Stream Destino");//Stream Destino que recibe la imagen sobel

	char imageNamesSrc[][40] = {
							"Test_Images/mono.bmp"
							};
	char imageNamesDst[][40] = {
							"Test_Images/monoABS.bmp"
							   };


	for (char k=0;k<sizeof(imageNamesDst)/sizeof(imageNamesDst[0]);++k)
	{
	printf("************Iniciando deteccion de bordes imagen %d********\n",k+1);

	pix_src = loadBmp(*(imageNamesSrc+k),&cabecera);//Almaceno los pixeles en el puntero pix_src

	for(int i=0; i<cabecera.height; ++i) //Leo toda la imagen pixel a pixel
		for(int j=0; j<cabecera.width; ++j)
		{
			pixel_out.data=pix_src[i*cabecera.width + j].g;
			pixel_out.strb=1;
			if(j==cabecera.width-1)
				pixel_out.last=1;
			else
				pixel_out.last=0;
			strm_src.write(pixel_out);
		}

	printImageAttrib(&cabecera);//Imprimir info de la imagen. Comentar si no se requiere

	edge_detect(strm_src,strm_dst,cabecera.height,cabecera.width); //Envío a la función detección de bordes

	pix_t *pix_dst = (pix_t*) malloc(cabecera.size);//Pido memoria para el puntero de la imagen destino

	for(int i=0; i<cabecera.height; ++i) //Recibo los pixeles procesados
			for(int j=0; j<cabecera.width; ++j)
				pix_dst[i*cabecera.width + j].r=pix_dst[i*cabecera.width + j].b=pix_dst[i*cabecera.width + j].g= strm_dst.read().data;

	saveBmp(*(imageNamesDst+k),&cabecera,pix_dst);//Guardo los pixeles procesados en una imagen

	free (pix_src);//Libero la memoria asignada al puntero pix_src
	free (pix_dst);//Libero la memoria asignada al puntero pix_dst

	printf("************proceso exitoso imagen %d********\n",k+1);
	}
	return 0;
}



