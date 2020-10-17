/* Autor: Roberto Millón
 * Universidad Nacional de chilecito
 * mail: rmillon@undec.edu.ar
 *
 * Recibo 3 uint (12 bytes) que corresponden a 4 pixeles (cada pixel de 3 bytes)
 *y envío el promedio de cada pixel para generar la imagen en escala de grises.
 *
 */


#include "rgb2gray.h"

void rgb2gray (streamU32_t& stm_in,streamU8_t& stm_out,short row_size,short col_size)
{
#pragma HLS INTERFACE s_axilite port=col_size bundle=CTRL_PORT
#pragma HLS INTERFACE s_axilite port=row_size bundle=CTRL_PORT
#pragma HLS INTERFACE axis register both port=stm_out
#pragma HLS INTERFACE axis register both port=stm_in
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_PORT

	unsigned short data_sum;
	unsigned int data_rcv;
	unsigned char data_rcv_u8[12];
#pragma HLS ARRAY_PARTITION variable=data_rcv_u8 complete dim=1

	unsigned char rem;
	dataU32_t data_in;
	dataU8_t data_out;

	row_loop:for (int row=0; row<ALTO_MAX; ++row )
	{
		col_loop:for(int col=0; col<ANCHO_MAX; ++col)
		{
#pragma HLS PIPELINE
			bounderies_if:if((row<row_size)&&(col < (col_size*3)/4))
			{

			data_in = stm_in.read();
			rem = col%3;
			data_rcv = data_in.data;
			for(char i=0; i<4; ++i)
				data_rcv_u8[rem*4+i] = (data_rcv >> 8*i) & 0xFF;
			if(rem == 2)
				avg_loop:for (char i=0; i<4; ++i)
				{
					data_sum = (data_rcv_u8[i*3]+data_rcv_u8[i*3+1]+data_rcv_u8[i*3+2])/3;
					data_out.data = (unsigned char)data_sum;
					data_out.keep = 1;
					data_out.strb = 1;

					if((col == col_size-1)&(i==3)&(row==row_size-1))
						data_out.last=1;
					else
						data_out.last=0;

					stm_out.write(data_out);
				}

			}//Fin bounderies_if

		}//Fin col_loop
	}//Fin row_loop
}












