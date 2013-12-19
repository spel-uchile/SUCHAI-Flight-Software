/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2013, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rs232_suchai.h"


/*------------------------------------------------------------------------------
 *		 		CONFIG RS232
 *------------------------------------------------------------------------------
 * Function Prototype : void ConfigRS232(unsigned int baudrate, unsigned int unsigned int)
 * Include            : uart.h - rs232_suchai.h
 * Description        : Confiugures UART periferal to operate with RS232 protocol
 *			using 8 bit data, no parity, 1 stop bit. Also set de baudrate
 * Arguments          : abaud - Calculated ABAUD value
                        unsigned int  - Module will operate
                                        RS2_M_UART1
 					RS2_M_UART2
 * Return Value      :  None
 * Remarks           :  This function configures the UARTx Control register, UARTx 
 *                    	Baud Rate Generator register and disable interruptse.
 *                   	NOTE: ABAUD=FCY/(PS*BAUDRATE)-1
 *                    	19200 bps => 51
 *			9600 bps => 103
 * *			1200 bps => 832
 *----------------------------------------------------------------------------*/
void ConfigRS232(unsigned int abaud, unsigned int modulo)
{
    /*
     * No se puede realizar en el PIC la oepración:
     * abaud = FCY/(PS*BAUDRATE)-1
     * al ser numeros mayores que MAX_INT.
     * Sol: Ingresar el valor abaud ya calculado
     */
    switch(modulo)
    {
        case RS2_M_UART1:

            /* LOS PUERTOS DE SALIDA DEBEN ESTAR MAPEADOS */

            CloseUART1();
            ConfigIntUART1(UART_RX_INT_DIS & UART_RX_INT_PR5 & UART_TX_INT_DIS & UART_TX_INT_PR0);
            OpenUART1(UART_EN & UART_IDLE_CON & UART_EN_WAKE & UART_DIS_LOOPBACK & UART_DIS_ABAUD & UART_NO_PAR_8BIT & UART_1STOPBIT & UART_IrDA_DISABLE & UART_MODE_SIMPLEX & UART_UEN_00 & UART_UXRX_IDLE_ONE & UART_BRGH_SIXTEEN,
                      UART_INT_TX & UART_IrDA_POL_INV_ZERO & UART_SYNC_BREAK_DISABLED & UART_TX_ENABLE & UART_INT_RX_CHAR & UART_ADR_DETECT_DIS & UART_RX_OVERRUN_CLEAR,
                      abaud); /* abaud = FCY/(PS*BAUDRATE)-1 => 16MIPS/(16*19200bps)-1=51  */


            /*Fuerza el overflow para borrar los buffers*/
            U1MODEbits.LPBACK = 1;
            while(U1STAbits.OERR == 0){
                    while(U1STAbits.UTXBF);WriteUART1('\0');
            }
            U1STAbits.OERR = 0;
            U1MODEbits.LPBACK = 0; //no more loop back

        break;
        case RS2_M_UART2:

            /* LOS PUERTOS DE SALIDA DEBEN ESTAR MAPEADOS */

            CloseUART2();
            ConfigIntUART2(UART_RX_INT_DIS & UART_RX_INT_PR5 & UART_TX_INT_DIS & UART_TX_INT_PR0);
            OpenUART2(UART_EN & UART_IDLE_CON & UART_EN_WAKE & UART_DIS_LOOPBACK & UART_DIS_ABAUD & UART_NO_PAR_8BIT & UART_1STOPBIT & UART_IrDA_DISABLE & UART_MODE_SIMPLEX & UART_UEN_00 & UART_UXRX_IDLE_ONE & UART_BRGH_SIXTEEN,
                      UART_INT_TX & UART_IrDA_POL_INV_ZERO & UART_SYNC_BREAK_DISABLED & UART_TX_ENABLE & UART_INT_RX_CHAR & UART_ADR_DETECT_DIS & UART_RX_OVERRUN_CLEAR,
                      abaud); /* abaud = FCY/(PS*BAUDRATE)-1 => 16MIPS/(16*19200bps)-1=51  */

            /*Fuerza el overflow para borrar los buffers*/
            U2MODEbits.LPBACK = 1;
            while(U2STAbits.OERR == 0){
                    while(U2STAbits.UTXBF);WriteUART2('\0');
            }
            U2STAbits.OERR = 0;
            U2MODEbits.LPBACK = 0; //no more loop back

        break;
        case RS2_M_UART3:

            /* LOS PUERTOS DE SALIDA DEBEN ESTAR MAPEADOS */

            CloseUART3();
            ConfigIntUART3(UART_RX_INT_DIS & UART_RX_INT_PR5 & UART_TX_INT_DIS & UART_TX_INT_PR0);
            OpenUART3(UART_EN & UART_IDLE_CON & UART_EN_WAKE & UART_DIS_LOOPBACK & UART_DIS_ABAUD & UART_NO_PAR_8BIT & UART_1STOPBIT & UART_IrDA_DISABLE & UART_MODE_SIMPLEX & UART_UEN_00 & UART_UXRX_IDLE_ONE & UART_BRGH_SIXTEEN,
                      UART_INT_TX & UART_IrDA_POL_INV_ZERO & UART_SYNC_BREAK_DISABLED & UART_TX_ENABLE & UART_INT_RX_CHAR & UART_ADR_DETECT_DIS & UART_RX_OVERRUN_CLEAR,
                      abaud); /* abaud = FCY/(PS*BAUDRATE)-1 => 16MIPS/(16*19200bps)-1=51  */

            /*Fuerza el overflow para borrar los buffers*/
            U3MODEbits.LPBACK = 1;
            while(U3STAbits.OERR == 0){
                    while(U3STAbits.UTXBF);WriteUART3('\0');
            }
            U3STAbits.OERR = 0;
            U3MODEbits.LPBACK = 0; //no more loop back

        break;
        case RS2_M_UART4:

            /* LOS PUERTOS DE SALIDA DEBEN ESTAR MAPEADOS */

            CloseUART4();
            ConfigIntUART4(UART_RX_INT_DIS & UART_RX_INT_PR5 & UART_TX_INT_DIS & UART_TX_INT_PR0);
            OpenUART4(UART_EN & UART_IDLE_CON & UART_EN_WAKE & UART_DIS_LOOPBACK & UART_DIS_ABAUD & UART_NO_PAR_8BIT & UART_1STOPBIT & UART_IrDA_DISABLE & UART_MODE_SIMPLEX & UART_UEN_00 & UART_UXRX_IDLE_ONE & UART_BRGH_SIXTEEN,
                      UART_INT_TX & UART_IrDA_POL_INV_ZERO & UART_SYNC_BREAK_DISABLED & UART_TX_ENABLE & UART_INT_RX_CHAR & UART_ADR_DETECT_DIS & UART_RX_OVERRUN_CLEAR,
                      abaud); /* abaud = FCY/(PS*BAUDRATE)-1 => 16MIPS/(16*19200bps)-1=51  */

            /*Fuerza el overflow para borrar los buffers*/
            U4MODEbits.LPBACK = 1;
            while(U4STAbits.OERR == 0){
                    while(U4STAbits.UTXBF);WriteUART4('\0');
            }
            U4STAbits.OERR = 0;
            U4MODEbits.LPBACK = 0; //no more loop back

        break;
    }
}

/*------------------------------------------------------------------------------
 *                                  SEND RS232
 *------------------------------------------------------------------------------
 * Function Prototype : void RS232EnableInterrupt(unsigned int modulo)
 * Include            : uart.h - rs232_suchai.h
 * Description        : Enables RX interrup with priority 5
 * Arguments          : modulo - Module will operate
 							RS2_M_UART1
 							RS2_M_UART2
 * Return Value      :  None
 *----------------------------------------------------------------------------*/
void RS232EnableRxInterrupt(unsigned int modulo)
{
    switch(modulo)
    {
        case RS2_M_UART1:
            ConfigIntUART1(UART_RX_INT_EN & UART_RX_INT_PR5 & UART_TX_INT_DIS
                            & UART_TX_INT_PR0);
            break;

        case RS2_M_UART2:
            ConfigIntUART2(UART_RX_INT_EN & UART_RX_INT_PR5 & UART_TX_INT_DIS
                            & UART_TX_INT_PR0);
            break;
        case RS2_M_UART3:
            ConfigIntUART3(UART_RX_INT_EN & UART_RX_INT_PR5 & UART_TX_INT_DIS
                            & UART_TX_INT_PR0);
            break;
        case RS2_M_UART4:
            ConfigIntUART4(UART_RX_INT_EN & UART_RX_INT_PR5 & UART_TX_INT_DIS
                            & UART_TX_INT_PR0);
            break;
    }
}

/*------------------------------------------------------------------------------
 *		 		SEND RS232
 *------------------------------------------------------------------------------
 * Function Prototype : void SendRS232(unsigned char* datos, unsigned int largo
 						, unsigned int modulo)
 * Include            : uart.h - rs232_suchai.h
 * Description        : Send a set of data bytes trhow RS232 protocol
 * Arguments          : datos - Pointer to array with data will be send
 						largo - Total data bytes will be read from array
 						modulo - Module will operate
 							RS2_M_UART1
 							RS2_M_UART2
 * Return Value      :  None
 * Remarks           :  This function write data to TX Buffer of UARTx module 
 						and send by	rs232.
 *----------------------------------------------------------------------------*/
void SendRS232(unsigned char* datos, unsigned int largo, unsigned int modulo)
{
    int i;
    switch(modulo){
        case RS2_M_UART1:
            for(i=0;i<largo;i++){
                while(U1STAbits.UTXBF);  /* wait if the buffer is full */
                WriteUART1(*(datos+i));
            }
        break;
        case RS2_M_UART2:
            for(i=0;i<largo;i++){
                while(U2STAbits.UTXBF);  /* wait if the buffer is full */
                WriteUART2(*(datos+i));
            }
        break;
        case RS2_M_UART3:
            for(i=0;i<largo;i++){
                while(U3STAbits.UTXBF);  /* wait if the buffer is full */
                WriteUART3(*(datos+i));
            }
        break;
        case RS2_M_UART4:
            for(i=0;i<largo;i++){
                while(U4STAbits.UTXBF);  /* wait if the buffer is full */
                WriteUART4(*(datos+i));
            }
        break;
    }
}

/*------------------------------------------------------------------------------
 *		 							SEND STR RS232
 *------------------------------------------------------------------------------
 * Function Prototype : void SendRS232(unsigned char* datos, unsigned int largo, unsigned int modulo)
 * Include            : uart.h - rs232_suchai.h
 * Description        : Send a set of data bytes trhow RS232 protocol
 * Arguments          : str - Pointer to array with data to be send. Sends untill \0 or \n is
 *                      detected.
 *						modulo - Module will operate
 *							RS2_M_UART1
 							RS2_M_UART2
 * Return Value      :  None
 * Remarks           :  This function write data to TX Buffer of UARTx module and send by
 						rs232. The function spect the string ends with '\0' or '\n'
 *----------------------------------------------------------------------------*/
void SendStrRS232(char* str, unsigned int modulo){
    int i = 0;
    switch(modulo){
        case RS2_M_UART1:
            while((str[i]>=0x20) || (str[i]=='\n') || (str[i]=='\r')){
                while(U1STAbits.UTXBF);  /* wait if the buffer is full */
                WriteUART1(str[i]);
                i++;
            }
        break;
        case RS2_M_UART2:
            while((str[i]>=0x20) || (str[i]=='\n') || (str[i]=='\r')){
                while(U2STAbits.UTXBF);  /* wait if the buffer is full */
                WriteUART2(str[i]);
                i++;
            }
        break;
        case RS2_M_UART3:
            while((str[i]>=0x20) || (str[i]=='\n') || (str[i]=='\r')){
                while(U3STAbits.UTXBF);  /* wait if the buffer is full */
                WriteUART3(str[i]);
                i++;
            }
        break;
        case RS2_M_UART4:
            while((str[i]>=0x20) || (str[i]=='\n') || (str[i]=='\r')){
                while(U4STAbits.UTXBF);  /* wait if the buffer is full */
                WriteUART4(str[i]);
                i++;
            }
        break;
    }
}

/*------------------------------------------------------------------------------
 *		 							READ RS232
 *------------------------------------------------------------------------------
 * Function Prototype : unsigned int  ReadRS232(unsigned int modulo)
 * Include            : uart.h - rs232_suchai.h
 * Description        : Read a byte from UARTx recive buffer
 * Arguments          : modulo - Module will operate
 							RS2_M_UART1
 							RS2_M_UART2
 * Return Value      :  return a byte read from rs232 recive buffer
 * Remarks           :  none
 *-----------------------------------------------------------------------------*/
unsigned int ReadRS232(unsigned int modulo)
{
    unsigned int data;
    switch(modulo){
        case RS2_M_UART1:
            data = ReadUART1();
        break;
        case RS2_M_UART2:
            data = ReadUART2();
        break;
        case RS2_M_UART3:
            data = ReadUART3();
        break;
        case RS2_M_UART4:
            data = ReadUART4();
        break;
    }
    return data;
}

/*------------------------------------------------------------------------------------------
 *                              READ STR RS232 ISR
 *------------------------------------------------------------------------------------------
 * Function Prototype : int ReadStrRS232_ISR(int *buff_count, char *buffer, int len, unsigned int modulo)
 * Include            : uart.h - rs232_suchai.h
 * Description        : Reads a string from UARTx and saves into a recive buffer
 *                      until a 'n' is recived or "len" characters were read
 * Arguments          : buff_count - Pointer to the counter of received chars
 *                      buffer - Pointer to he serial buffer
 *                      len - Lenght of the buffer or number of expected chars
 *                      modulo - Module will operate
 * 							RS2_M_UART1
 *							RS2_M_UART2
 * Return Value      :  Return buff_count value
 *                          >0 - More chars are expected
 *                           0 - The string was received
 * Remarks           :  none
 *----------------------------------------------------------------------------------------*/
int ReadStrRS232_ISR(int *buff_count, char *buffer, int len, unsigned int modulo)
{
    char ch;
    switch(modulo)
    {
        case RS2_M_UART1:
            ch = ReadUART1();
        break;
        case RS2_M_UART2:
            ch = ReadUART2();
        break;
        case RS2_M_UART3:
            ch = ReadUART3();
        break;
        case RS2_M_UART4:
            ch = ReadUART4();
        break;
        default:
            return 0;
        break;
    }

    if(*buff_count<len)
        buffer[(*buff_count)++] = ch;

    if((*buff_count >= len) || (ch == '\n') || (ch == '\r'))
    {
        *buff_count = 0;
    }
    return *buff_count;
}

/*------------------------------------------------------------------------------
 *                                      READ FRMAE RS232
 *------------------------------------------------------------------------------
 * Function Prototype : int ReadStrRS232_ISR(int *buff_count, char *buffer,
 *                                           int len, unsigned int modulo)
 * Include            : uart.h - rs232_suchai.h
 * Description        : Reads a frame from UARTx and saves into a recive buffer
 *                      "len" bytes were read
 * Arguments          : buff_count - Pointer to the counter of received chars
 *                      buffer - Pointer to he serial buffer
 *                      len - Lenght of the buffer or number of expected chars
 *                      modulo - Module will operate
* 				RS2_M_UART1
*				RS2_M_UART2
 * Return Value      :  Return buff_count value
 *                          >0 - More chars are expected
 *                           0 - The string was received
 * Remarks           :  none
 *----------------------------------------------------------------------------------------*/
int ReadFrameRS232_ISR(int *buff_count, char *buffer, int len, unsigned int modulo)
{
    char ch;
    switch(modulo)
    {
        case RS2_M_UART1:
            ch = ReadUART1();
        break;
        case RS2_M_UART2:
            ch = ReadUART2();
        break;
        case RS2_M_UART3:
            ch = ReadUART3();
        break;
        case RS2_M_UART4:
            ch = ReadUART4();
        break;
        default:
            return 0;
        break;
    }

    if(*buff_count<len)
        buffer[(*buff_count)++] = ch;

    if(*buff_count >= len)
    {
        *buff_count = 0;
    }
    return *buff_count;
}

void EchoRS232(unsigned int modulo)
{
    switch(modulo){
        case RS2_M_UART1:
            while(DataRdyUART1())
                WriteUART1(ReadUART1());
        break;
        case RS2_M_UART2:
            while(DataRdyUART2())
                WriteUART2(ReadUART2());
        break;
        case RS2_M_UART3:
            while(DataRdyUART3())
                WriteUART3(ReadUART3());
        break;
        case RS2_M_UART4:
            while(DataRdyUART4())
                WriteUART4(ReadUART4());
        break;
    }
}
