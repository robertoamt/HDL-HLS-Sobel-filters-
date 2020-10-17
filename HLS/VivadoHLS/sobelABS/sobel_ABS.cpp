/* Autor: Roberto Millón
 * Universidad Nacional de chilecito
 * mail: rmillon@undec.edu.ar
 *
 * Función para detectar lo bordes en una imagen por algoritmo Sobel
 */


#include "sobel_ABS.h"

void edge_detect (stream_t& strm_in,stream_t& strm_out,int nro_filas, int nro_col)
{
#pragma HLS INTERFACE s_axilite port=nro_col bundle=CTRL_PORT
#pragma HLS INTERFACE s_axilite port=nro_filas bundle=CTRL_PORT
#pragma HLS INTERFACE axis register both port=strm_out
#pragma HLS INTERFACE axis register both port=strm_in
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_PORT
	pixel_t pixel_out;
	unsigned char pixel_in;
	unsigned char window [WIN_SIZE][WIN_SIZE];
#pragma HLS ARRAY_PARTITION variable=window complete dim=1
	unsigned char pix, last_pix, send_pix;
	unsigned char right_column [WIN_SIZE];
#pragma HLS ARRAY_PARTITION variable=right_column complete dim=1
	unsigned char line_buffer [WIN_SIZE][ANCHO_MAX];
#pragma HLS ARRAY_PARTITION variable=line_buffer block factor=3

	Loop_Row:for(int row=0; row<ALTO_MAX; ++row)//Me desplazo por las filas de la imgen
	{
		Loop_Col:for(int col=0; col<ANCHO_MAX; ++col)//Me desplazo por las columnas de la imagen
		{
#pragma HLS PIPELINE II=1 rewind

			if((col<nro_col)&&(row<nro_filas))//Controla la cantidad de pixeles de la imagen recibida (cantidad -> ancho de la imagen)
			{
				pixel_in = strm_in.read().data;
				for(unsigned char k=0; k<WIN_SIZE-1;++k) //Desplazo toda la columna hacia arriba
					right_column[k]=line_buffer[k][col]=line_buffer[k+1][col];

				right_column[WIN_SIZE-1]=line_buffer [WIN_SIZE-1][col]=pixel_in;//En la ultima posición de la columna actual del line buffer almaceno el pixel de la imagen


			Loop_Row_Win:for (unsigned char i=0; i<WIN_SIZE; ++i)
			{
				Loop_Col_Win:for(unsigned char j=0; j<WIN_SIZE-1; ++j) //Desplazo la ventana hacia la izquierda
					window[i][j] = window[i][j+1];

				window[i][WIN_SIZE-1]=right_column[i];//Almaceno la columna de la derecha de line buffer (columna con el nuevo pixel)
			}//End Loop_Row_Win

			if((row>=WIN_SIZE-1)&&(col>=WIN_SIZE-1))//Envío los pixeles convolucionados que estan en este rango
								send_pix=convolut(window);
							else
								send_pix=0;

							pixel_out.data=send_pix;
							pixel_out.keep=1;

							if((col==nro_col-1)&(row==nro_filas-1))
								pixel_out.last=1;
							else
								pixel_out.last=0;

							strm_out.write(pixel_out);


			}//End if
		}//End Loop_col
	}//End Loop_Row
}

///////Función que obtiene el valor del borde de la imagen
unsigned char convolut(unsigned char window [][WIN_SIZE])
{

	const char x_kernel [][WIN_SIZE]={ {-1,0,1},
	        						   {-2,0,2},
									   {-1,0,1}};
#pragma HLS ARRAY_PARTITION variable=x_kernel complete dim=0


	const char y_kernel [][WIN_SIZE]={ {-1,-2,-1},
									   {0,0,0},
									   {1,2,1}};
#pragma HLS ARRAY_PARTITION variable=y_kernel complete dim=0


	short x_result;
	short y_result;
	float total_result;
	unsigned char value;



	x_result = 0;
	y_result = 0;

	sobel_kernel_label1:for(unsigned char i=0; i<WIN_SIZE; ++i)

sobel_kernel_label0:for(unsigned char j=0; j<WIN_SIZE; ++j)
		{
			x_result+=x_kernel[i][j]*window[i][j];
			y_result+=y_kernel[i][j]*window[i][j];
		}

		total_result = (ABS(x_result)+ ABS(y_result));
	        value = (unsigned char)total_result;

	return (value);
}
