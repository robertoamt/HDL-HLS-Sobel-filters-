# Sobel_HLS_ZYBO

Detector de contornos SOBEL en SoC Zybo de Xilinx.

El proyecto se desarrolló utilizando la plataforma de desarrollo ZYBO y las herramientas de Xilinx. La placa ZYBO se basa en la arquitectura AP SoC y está integrada por un procesador de doble núcleo ARM Cortex-A9 y una FPGA XC7Z010-1-CLG400C. 
Se diseñaron dos versiones de filtros Sobel (VHDL y HLS). Los bloques de alto nivel se sintetizaron en Vivado HLS 2019.1 y los bloques HDL en vivado 2019.1.
La integración del sistema se realizó también en Vivado 2019.1 e incluye un DMA, un conversor de imágen a color en escala de grises, el filtro sobel y un bloque adicional 
para unir bytes en palabras de 32 bits.
Las pruebas reales se realizaron con dos imágenes obtenidas de repositorios públicos denominadas mono y buho. La aplicación de prueba corre en la herramienta XSDK 2019.1.

El diseño permite procesar imágenes de hasta 1920 x 1080 píxeles (Full HD) pero se puede extender. Se recomienda utilizar Vivado 2019.1 para evitar problemas de compatibilidad

Pasos para utilizar el proyecto

1) En vivado HLS sintetizar y exportar los bloques HLS (sobelABS, rgb2gray y u8tou32).

2) En vivado sintetizar y exportar los bloques HDL (sobel, rgb2gray y u8tou32) 

3) En vivado 2019.1, crear un proyecto nuevo y agregar los repositorios de los seis ip generados en los puntos 1) y 2)

4) Ejecutar system.tcl (source <path to file>/system.tcl)

5) Modificar el proyecto al cambiar los bloques HLS por los bloques HDL o visceversa.
  
6) Crear la envoltura HDL al diseño de bloques (HDL wrapper) y generar el bitstream

7) Exportar el hardware y ejecutar XSDK

8) Crear un proyecto de apliación vacío con el nombre edgeDetect e importar los archivos "cabecera.h" y "edgeDetect.c"

9) En el archivo cabecera elegir los bloques incorporados al sistema (comentar los que no se usarán)

10) Modificar el paquete de soporte de la placa (Board Support Package) e incorporar la librería xilffs (Generic Fat File System Library)

11) Almacenar las imágenes de prueba en una microSD. Incorporar la microSD en la placa ZYBO. 

12) Conectar la placa ZYBO a la pc y ejecutar la aplicación edgeDetect. 








