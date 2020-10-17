/*
 * cabecera.h
 *
 *  Created on: 15-05-2020
 *      Author: roberto
 */

#ifndef CABECERA_H_
#define CABECERA_H_

typedef unsigned char byte_t;
typedef unsigned short byte2_t;
typedef unsigned int byte4_t;
typedef struct {unsigned char r, g, b;} pix_t;
//typedef struct pp pix_t;

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

/*------HLS cores-------*/
//#define rgb2gray
//#define sobel
//#define u8tou32
/*----------------------*/

/*------HDL cores-------*/
#define u8tou32VHDL
#define sobelVHDL
#define rgb2grayVHDL
/*----------------------*/



#define DMA_DEV_ID		XPAR_AXIDMA_0_DEVICE_ID
#define MEM_BASE_ADDR		0x01000000
#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00700000)
#define RX_BUFFER_HIGH		(MEM_BASE_ADDR + 0x00CFFFFF)

extern void xil_printf(const char *format, ...);
int filterSobel3x3(u16 DeviceId);
static int readFile(void);
static int writeFile(void);
static void initPerip(void);


#endif /* SRC_CABECERA_H_ */





