#ifndef _DEBUG_INCLUDES_H
#define _DEBUG_INCLUDES_H

#include <stdio.h>  //para sprintf
#include <stdlib.h>  //para itoa

#include "rs232_suchai.h"

#define con_putc(c) SendRS232((unsigned char *)(c),1,RS2_M_UART1)
#define con_putcl(c) while(U1STAbits.UTXBF);WriteUART1(c);
//#define con_printf(c) SendStrRS232(c,RS2_M_UART1)
#define con_getc() ((char)ReadRS232(RS2_M_UART1))
#define con_strncpy(a,b,n) strncpy(a,b,n); *((a)+(n))='\0';

void con_printf(char *c);

#endif /* _DEBUG_INCLUDES_H */
