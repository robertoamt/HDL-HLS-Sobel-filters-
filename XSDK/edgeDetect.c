
/***************************** Include Files *********************************/
#include "xaxidma.h"
#include "xparameters.h"
#include "xdebug.h"
#include "ff.h"
#include "cabecera.h"
#include "xtime_l.h"
#include "stdio.h"


#ifdef rgb2gray
#include "xrgb2gray.h"
#endif

#ifdef u8tou32
#include "xu8tou32.h"
#endif

#ifdef sobel
#include "xedge_detect.h"
#endif


/******************** Instances Definitions **********************************/

static FIL fil;
static FATFS fatfs;
cabecera_t cabecera_inst;
FRESULT Res;
UINT NumBytesRead;
TCHAR *Path = "0:/";
int resultSDcard;
u32 IMG_SIZE;
XAxiDma AxiDma;


XTime tStart,tbeforeSobel,tafterSobel, tEnd,tafterRead,tbeforeRead,tafterWrite,tbeforeWrite,tafterInit,tbeforeInit,tbeforeGlobalS,tafterGlobalS;


#ifdef rgb2gray
XRgb2gray rgbInst;
#endif

#ifdef u8tou32
XU8tou32 u8Inst;
#endif

#ifdef sobel
XEdge_detect sobelInst;
#endif




/******************** Constant Definitions **********************************/

static char srcImg[] = "buho.bmp";
static char dstImg[] = "buho2.bmp";


int main()
{

	int Status;

	//xil_printf("\r\n--- Entering main() --- \r\n");

	XTime_GetTime(&tStart);//Obtengo el tiempo inicial

	Status=readFile(); //Leo imagen

	XTime_GetTime(&tafterRead);

	if (Status != XST_SUCCESS) {
			xil_printf("Error en la lectura de imagen\r\n");
			return XST_FAILURE;
		}

	XTime_GetTime(&tbeforeInit);
	initPerip(); //Inicializo perifericos
	XTime_GetTime(&tafterInit);
	//xil_printf("filas %d\r\n",(u32)cabecera_inst.height);
	//xil_printf("columnas %d\r\n",(u32)cabecera_inst.width);
	Status = filterSobel3x3(DMA_DEV_ID); // Ejecuto Sobel
	if (Status != XST_SUCCESS) {
		xil_printf("Error en la ejecucion de Sobel\r\n");
		return XST_FAILURE;
	}

	XTime_GetTime(&tbeforeWrite);
	Status=writeFile(); //Guardo Imagen
	XTime_GetTime(&tafterWrite);

	if (Status != XST_SUCCESS) {
				xil_printf("Error en la escritura de imagen\r\n");
				return XST_FAILURE;
			}

	//xil_printf("--- Exiting main() --- \r\n");

	XTime_GetTime(&tEnd);//Obtengo el tiempo Final

	printf("Total %.2f us, Read %.2f us, Init %.2f us, Global Sobel %.2f us, Sobel %.2f us, Write %.2f us\r\n",\
			1.0*(tEnd-tStart)/(COUNTS_PER_SECOND/1000000),\
			1.0*(tafterRead-tStart)/(COUNTS_PER_SECOND/1000000),\
			1.0*(tafterInit-tbeforeInit)/(COUNTS_PER_SECOND/1000000),\
			1.0*(tbeforeWrite-tafterInit)/(COUNTS_PER_SECOND/1000000),\
			1.0*(tafterSobel-tbeforeSobel)/(COUNTS_PER_SECOND/1000000),\
			1.0*(tafterWrite-tbeforeWrite)/(COUNTS_PER_SECOND/1000000));

	return XST_SUCCESS;

}





static int readFile(void)
{
		//Leer imagen y guardar pixeles en en arreglo (sourceAdress)

		resultSDcard=0;
		Res=0;
		Res += f_mount(&fatfs, Path, 0);
		Res += f_open(&fil, srcImg, FA_OPEN_ALWAYS | FA_READ);
		Res += f_lseek(&fil, 0);
		Res += f_read(&fil, (void*)&cabecera_inst, sizeof(cabecera_t),&NumBytesRead);
		Res += f_lseek(&fil, cabecera_inst.offset);
		Res += f_read(&fil, (void*)TX_BUFFER_BASE, cabecera_inst.imageSize, &NumBytesRead);
		Res += f_close(&fil);
		if (Res == FR_OK)
				return XST_SUCCESS;
				//xil_printf("Lectura de imagen exitosa\n\r");
		else
				return XST_FAILURE;
}

static void initPerip(void)
{
#ifdef rgb2gray
	XRgb2gray_Initialize(&rgbInst,XPAR_RGB2GRAY_0_DEVICE_ID);
	XRgb2gray_InterruptGlobalDisable(&rgbInst);
	XRgb2gray_InterruptDisable(&rgbInst,0x00000003);
	XRgb2gray_Set_row_size(&rgbInst,(u32)cabecera_inst.height);
	XRgb2gray_Set_col_size(&rgbInst,(u32)cabecera_inst.width);
	XRgb2gray_EnableAutoRestart(&rgbInst);
	XRgb2gray_Start(&rgbInst);
#endif

#ifdef u8tou32
	XU8tou32_Initialize(&u8Inst,XPAR_U8TOU32_0_DEVICE_ID);
	XU8tou32_InterruptGlobalDisable(&u8Inst);
	XU8tou32_InterruptDisable(&u8Inst,0x00000003);
	XU8tou32_Set_row_size(&u8Inst,(u32)cabecera_inst.height);
	XU8tou32_Set_col_size(&u8Inst,(u32)cabecera_inst.width);
	XU8tou32_EnableAutoRestart(&u8Inst);
	XU8tou32_Start(&u8Inst);
#endif

#ifdef sobel
	XEdge_detect_Initialize(&sobelInst,XPAR_EDGE_DETECT_0_DEVICE_ID);
	XEdge_detect_InterruptGlobalDisable(&sobelInst);
	XEdge_detect_InterruptDisable(&sobelInst,0x00000003);
	XEdge_detect_EnableAutoRestart(&sobelInst);
	XEdge_detect_Set_nro_filas(&sobelInst,(u32)cabecera_inst.height);
	XEdge_detect_Set_nro_col(&sobelInst,(u32)cabecera_inst.width);
	XEdge_detect_Start(&sobelInst);
#endif

#ifdef rgb2grayVHDL
	Xil_Out32(XPAR_RGB2GRAYHDL_0_BASEADDR,(u32) cabecera_inst.height);
	Xil_Out32(XPAR_RGB2GRAYHDL_0_BASEADDR,(u32)cabecera_inst.width);
#endif

#ifdef sobelVHDL
	Xil_Out32(XPAR_SOBELHDL_ABS_0_BASEADDR,(u32) cabecera_inst.height);
	Xil_Out32(XPAR_SOBELHDL_ABS_0_BASEADDR,(u32)cabecera_inst.width);
#endif

#ifdef u8tou32VHDL
	Xil_Out32(XPAR_U8TOU32HDL_0_BASEADDR,(u32)cabecera_inst.height);
	Xil_Out32(XPAR_U8TOU32HDL_0_BASEADDR,(u32)cabecera_inst.width);
#endif
}

static int writeFile(void)
{
	Res=0;
	Res += f_open(&fil, dstImg, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
	Res += f_lseek(&fil, 0);
	Res += f_write(&fil, (void*)&cabecera_inst, sizeof(cabecera_t),&NumBytesRead);
	Res += f_lseek(&fil, cabecera_inst.offset);
	Res += f_write(&fil, (void*)RX_BUFFER_BASE, cabecera_inst.imageSize,&NumBytesRead);
//	if(Res== FR_OK)
//				xil_printf("Escritura de imagen exitosa\n\r");
	Res += f_close(&fil);
	Res += f_unmount(Path);
	if (Res == FR_OK)
		return XST_SUCCESS;
		//xil_printf("Disco desmontado\r\n");

	else
		return XST_FAILURE;
}



int filterSobel3x3(u16 DeviceId)
{
	XAxiDma_Config *CfgPtr;
	int Status;

	u8 *TxBufferPtr;
	u8 *RxBufferPtr;

	TxBufferPtr = (u8 *)TX_BUFFER_BASE ;
	RxBufferPtr = (u8 *)RX_BUFFER_BASE;
	IMG_SIZE = (u32)(3*cabecera_inst.height*cabecera_inst.width);

	//	Inicializo DMA
	CfgPtr = XAxiDma_LookupConfig(DeviceId);
	if (!CfgPtr) {
		xil_printf("No existe el dispositivo %d\r\n", DeviceId);
		return XST_FAILURE;
	}

	Status = XAxiDma_CfgInitialize(&AxiDma, CfgPtr);
	if (Status != XST_SUCCESS) {
		xil_printf("Error en la inicializacion del DMA %d\r\n", Status);
		return XST_FAILURE;
	}

	if(XAxiDma_HasSg(&AxiDma)){
		xil_printf("Error, dispositivo configurado en modo SG \r\n");
		return XST_FAILURE;
	}

	//	Deshabilito interrupciones
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,
						XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,
						XAXIDMA_DMA_TO_DEVICE);


	Xil_DCacheFlushRange((UINTPTR)TxBufferPtr, IMG_SIZE);



		XTime_GetTime(&tbeforeSobel); //Obtengo el tiempo antes de ejecutar Sobel
		//for (int i=0;i<10;++i)
		//{

		Status = XAxiDma_SimpleTransfer(&AxiDma,(UINTPTR) RxBufferPtr,
				IMG_SIZE, XAXIDMA_DEVICE_TO_DMA);

		if (Status != XST_SUCCESS)
			return XST_FAILURE;


		Status = XAxiDma_SimpleTransfer(&AxiDma,(UINTPTR) TxBufferPtr,
				IMG_SIZE, XAXIDMA_DMA_TO_DEVICE);

		if (Status != XST_SUCCESS)
			return XST_FAILURE;



		while ((XAxiDma_Busy(&AxiDma,XAXIDMA_DEVICE_TO_DMA)) ||
			(XAxiDma_Busy(&AxiDma,XAXIDMA_DMA_TO_DEVICE))) {/* Wait */}
		//}

		XTime_GetTime(&tafterSobel); //Obtengo el tiempo despues de ejecutar Sobel

		Xil_DCacheInvalidateRange((UINTPTR)RX_BUFFER_BASE, IMG_SIZE);
		//xil_printf("Sobel exitoso, iteracion %d\r\n",i);



	return XST_SUCCESS;
}

