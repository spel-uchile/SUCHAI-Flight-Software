#ifndef RS232_SUCHAI_H
#define RS232_SUCHAI_H

/******************************************************************************
 *
 *                  RS232 SUCHAI PERIPHERAL INTERFACE HEADER FILE
 *
 ******************************************************************************
 * FileName:        rs232_suchai.h
 * Dependencies:    uart.h
 * Processor:       PIC24F
 * Compiler:        MPLAB C30
 * Company:         SUCHAI
 *****************************************************************************/
#include <uart.h>
//#include <stdio.h> 
  
#define RS2_ENABLE_OUTPUT_MHX 	_TRISC1 = 0; _TRISE2 = 0; _TRISE3 = 0;
                                /* -OE_USB salida -OE_MHX salida -ON_MHX salida */
							
#define RS2_ACTIVATE_COMM_MHX	_LATC1 = 1; _LATE2 = 0; _LATE3 = 0;
				/* -OE_USB OFF -OE_MHX ON -ON_MHX ON */
							
#define RS2_OFF_OE_USB 			_LATC1 = 1		/* -OE_USB OFF */
#define RS2_ON_OE_USB			_LATC1 = 0 		/* -OE_USB ON */
#define RS2_OFF_OE_MHX 			_LATE2 = 1 		/* -OE_USB OFF */
#define RS2_ON_OE_MHX			_LATE2 = 0 		/* -OE_USB ON */
#define RS2_OFF_ON_MHX 			_LATE3 = 1 		/* -OE_USB OFF */
#define RS2_ON_ON_MHX			_LATE3 = 0 		/* -OE_USB ON */

/* Define diferencia entre mdulos UART  */
#define RS2_M_UART1     0	// Usar modulo UART1
#define	RS2_M_UART2	1	// Usar modulo UART2
#define	RS2_M_UART3	2	// Usar modulo UART3
#define	RS2_M_UART4	3	// Usar modulo UART4

/*------------------------------
 * 	    ABAUD VALUES TABLE
 *------------------------------
 * Baudrate		FCY		Abaud
 * -----------------------------
 *  500K		16MHz	1
 *  56K			16MHZ	17
 *  38.4K		16MHz	25
 *  19200		16MHz	51
 *  9600		16MHz	103
 *  2400		16MHz	416
 *------------------------------*/
 
void ConfigRS232(unsigned int baudrate, unsigned int modulo);
void SendRS232(unsigned char* datos, unsigned int largo, unsigned int modulo);
void SendRS232(unsigned char* datos, unsigned int largo, unsigned int modulo);
void SendStrRS232(char* str, unsigned int modulo);
unsigned int ReadRS232(unsigned int modulo);
int ReadStrRS232_ISR(int *buff_count, char *buffer, int len, unsigned int modulo);
int ReadFrameRS232_ISR(int *buff_count, char *buffer, int len, unsigned int modulo);
void EchoRS232(unsigned int modulo);
void RS232EnableRxInterrupt(unsigned int modulo);
 
 
#endif //RS232 SUCHAI_H
