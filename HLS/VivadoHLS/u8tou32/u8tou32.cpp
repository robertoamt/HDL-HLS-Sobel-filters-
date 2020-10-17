/* Autor: Roberto Millón
 * Universidad Nacional de chilecito
 * mail: rmillon@undec.edu.ar
 *
 * Esta función recibe paquetes de 1 byte, asigna ese byte a cada canal del RGB (triplica)
 * y envía paquetes de 4 bytes (32 bits para trabajar con DMA)
 *
 *
 *
 **/

#include "u8tou32.h"

void u8tou32 (streamU8_t& stm_in,streamU32_t& stm_out,short row_size,short col_size)
{
#pragma HLS INTERFACE s_axilite port=col_size bundle=CTRL_PORT
#pragma HLS INTERFACE s_axilite port=row_size bundle=CTRL_PORT
#pragma HLS INTERFACE axis register both port=stm_out
#pragma HLS INTERFACE axis register both port=stm_in
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_PORT

	unsigned char data_rcv[12];
#pragma HLS ARRAY_PARTITION variable=data_rcv complete dim=1
	unsigned int *intPtr = (unsigned int *)data_rcv;
	unsigned char rem;
	dataU8_t data_in;
	dataU32_t data_out;

	row_loop:for (int row=0; row<ALTO_MAX; ++row )
	{
		col_loop:for(int col=0; col<ANCHO_MAX; ++col)
		{
#pragma HLS PIPELINE
			bounderies_if:if((row<row_size)&&(col<col_size))
			{
			data_in = stm_in.read();
			rem = col%4;
			data_rcv[3*rem] = data_in.data;
			data_rcv[3*rem+1] = data_in.data;
			data_rcv[3*rem+2] = data_in.data;

			if(rem == 3)
			{
				u8tou32_label0:for(char i=0; i<3; ++i )
				{
				data_out.data = *(intPtr+i);
				data_out.strb=0xF;
				data_out.keep=0xF;

				if((col == col_size-1)&(i==2)&(row == row_size-1))
					data_out.last=1;
				else
					data_out.last=0;

				stm_out.write(data_out);
				}
			}
		  }//Fin bounderies_if
		}//Fin col_loop
	}//Fin row_loop
}












