/* Autores: Emmanuel Frat-Roberto Millón
 * Universidad Nacional de chilecito
 * mail: fefrati@undec.edu.ar-rmillon@undec.edu.ar
 */


#include "bmpTools.h"

pix_t *loadBmp(char *imageName, cabecera_t *imageHeader)
{
	FILE *imgSrc;
	imgSrc=fopen(imageName,"rb");
	if(imgSrc==NULL)
	{
		printf("No se pudo abrir la imagen\n\r");
		exit(-1);
	}
	rewind (imgSrc);
	if(fread(imageHeader,sizeof(cabecera_t), 1, imgSrc) != 1) //Leo la cabecera de la imagen
	{
		printf("Error al leer cabecera");
		exit(-1);
	}

	fseek(imgSrc, imageHeader->offset, SEEK_SET);

	int nro_filas = imageHeader->height;
	int nro_col= imageHeader->width;
	int nro_pixeles = nro_filas * nro_col;
	int size = sizeof(pix_t) * nro_pixeles;
	pix_t *pix_src = (pix_t*) malloc(size);//Pido memoria para el puntero de la imagen fuente

	if(fread(pix_src, size, 1,imgSrc)!=1)
	{
		printf("error al leer de nuevo la imagen\n");
		exit(-1);
	}
	fclose(imgSrc); //Cierro imagen

	return (pix_src);
}

void printImageAttrib(cabecera_t * cabecera)
{
	printf("Filas %d\n",cabecera->height);
	printf("Columnas %d\n",cabecera->width);
//	printf("Canales por pixel %d\n",sizeof(pix_t));
//	printf("Tamanio cabecera %d\n",sizeof(cabecera_t));
//	printf("Tamanio cabecera %d\n",cabecera.headerSize);
//	printf("offset %d\n",cabecera.offset);
//	printf("Tamanio imagen %d\n",cabecera.imageSize);
//	printf("Tamanio imagen %d\n",size);
}

void saveBmp(char*imageName,cabecera_t*imageHeader,pix_t *imagePixel)
{
	FILE *imgDst;
	imgDst = fopen(imageName,"wb"); //abro la imagen para escritura

	if (fwrite (imageHeader, sizeof(cabecera_t),1,imgDst)!= 1) //Copio la cabecera de la imagen fuente en la imagen destino
		{
			printf("Error escribiendo cabecera.\n");
			exit(-1);
		}

		fseek(imgDst, imageHeader->offset, SEEK_SET);
		if (fwrite(imagePixel,imageHeader->size,1, imgDst)!= 1) //Copio los pixeles del puntero que almacenó los pixeles recibidos
		{
			printf("Error escribiendo puntos.\n");
			exit(-1);
		}
		fclose(imgDst); //Cierro imagen

}



