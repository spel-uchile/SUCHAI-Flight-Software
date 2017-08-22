/*                                 I2C DRIVER
 *                          FOR MICROCHIP PIC24F MCUs
 *
 *      Copyright 2013, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU LESSER GENERAL PUBLIC LICENSE for more details.
 *
 * You should have received a copy of the GNU LESSER GENERAL PUBLIC LICENSE
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "i2c_comm.h"
//#include "console_tasks.h"

//Static functions
static void i2c_master_wait(int device);
static int i2c_master_wait_and_check(int device, int check_ack);
static int i2c_check_ack(int device);
static int i2c_check_bus_collision(int device);

static void i2c_slave_putc(char data);
static char i2c_slave_getc(void);

/**
 * Wait method selection. Uncomment only one define statement
 */
//#define _I2C_BUSY_WAIT ///< Uses busy waiting without ISR
#define _I2C_PWRS_WAIT ///< Uses IDLE mode waiting with ISR
//#define _I2C_RTOS_WAIT ///< Uses FreeRTOS semaphore syncronization with ISR

/**
 * Slave reception mode. Uncomment only one define statement
 */
//#define _I2C_SLAVE_RTOS ///< Uses Queue to store slave recived data
#define _I2C_SLAVE_BUFF ///< Uses buffer in ram to store recived data

#ifdef _I2C_SLAVE_RTOS
#endif

#ifdef _I2C_SLAVE_BUFF
#define _I2C_RCV_BUFF_LEN   10  ///< Slave recive buffer lenght
int i2c_slave_address = 0;      ///< Salve r/w selected address
char I2C_BUFF[_I2C_RCV_BUFF_LEN]; ///< Salve data buffer
#endif

int I2C_SLAVE_ST = I2C_SLV_IDLE;

/**
 * Confiugures I2C periferal to operate at user defined frecuency, iterrups are
 * configured depending of the wait method selected.
 *
 * @param BRG Computed value for the baud rate generator:
 *      Req.F  -  FCY  - BRG
 *      100kHz - 16MHz - 157
 *      100kHz - 08MHz - 78
 *      400kHz - 16MHz - 37
 *      001MHz - 16MHz - 13
 */
void i2c1_open(unsigned int BRG, char address)
{
#ifndef _I2C_RTOS_WAIT
    ConfigIntI2C1(MI2C_INT_OFF & MI2C_INT_PRI_0 & SI2C_INT_OFF & SI2C_INT_PRI_0); /* Software mode state trantition */
#else
    ConfigIntI2C1(MI2C_INT_ON & MI2C_INT_PRI_6 & SI2C_INT_ON & SI2C_INT_PRI_5); /* Interrupt mode state trantition */
#endif

    OpenI2C1(I2C_ON & I2C_IDLE_CON & I2C_CLK_HLD & I2C_IPMI_DIS & I2C_7BIT_ADD &
            I2C_SLW_EN & I2C_SM_DIS & I2C_GCALL_DIS & I2C_STR_EN & I2C_ACK &
            I2C_ACK_DIS & I2C_RCV_DIS & I2C_STOP_DIS & I2C_RESTART_DIS & I2C_START_DIS,
            BRG) ; /* BRG according to I2C Clock Rate table*/

    I2C2ADD = (unsigned int)address;
}

/**
 * Send @num bytes from @data to I2C slave device at @address.
 *
 * @param data Out buffer of size equal or major than @len.
 * @param address Buffer of address of slave I2C device and register to read.
 *        For  8bit register address   -> {dev_add, reg_add}
 *        For 16bit register address   -> {dev_add, reg_add_h, reg_add_l}
 * @param addlen For 8 bit register addres uses @addlen=2, for 16bit register
 * address use @addlen=3.
 *
 * @return Number of send bytes, if differs from @len an error ocurred.
 */
int i2c1_master_fputs(const char *data, int len, char *address, int addlen)
{
    register int ok = 0;
    register int count = 0;
    char w_address = address[0]<<1;                //Address+W

    //Waits while bus is busy
    IdleI2C1();

    //Init session sending Start condition
    StartI2C1();            
    ok = i2c_master_wait_and_check(I2C_MOD1, 0);   //Wait op. ends and check errors
    if(!ok) return count;                          //Return in error

    //Send device address
    MasterWriteI2C1(w_address);
    ok = i2c_master_wait_and_check(I2C_MOD1, 1);
    if(!ok) return count;

    //Send byte address
    for(count=1; count<addlen; count++)
    {
        MasterWriteI2C1(address[count]);
        ok = i2c_master_wait_and_check(I2C_MOD1, 1);
        if(!ok) return 0;
    }

    //Start sending data
    for(count=0; count<len; count++)
    {
        MasterWriteI2C1(data[count]);
        ok = i2c_master_wait_and_check(I2C_MOD1, 1);
        if(!ok) return count;
    }

    //Close session sending stop condition
    StopI2C1();
    i2c_master_wait_and_check(I2C_MOD1, 0); //TODO: No revisar ACK

    return count;
}

/**
 * Recive @len bytes from I2C slave device at @address and stores the result in
 * the buffer pointed by @data.
 *
 * @param data Out buffer of size equal or major than @len.
 * @param len Number of bytes to read.
 * @param address Buffer of address of slave I2C device and register to read.
 *        For  8bit register address   -> {dev_add, reg_add}
 *        For 16bit register address   -> {dev_add, reg_add_h, reg_add_l}
 * @param addlen For 8 bit register addres uses @addlen=2, for 16bit register
 * address use @addlen=3.
 *
 * @return Number of read bytes
 */
int i2c1_master_fgets(char *data, int len, char *address, int addlen)
{
    //STEP1 -> Set read direcction
    register int ok = 0;
    register int count = 0;
    char w_address = address[0]<<1;                //Address+W
    char r_address = w_address+1;                  //Address+R

    //Waits while bus is busy
    IdleI2C1();

    //Init session sending Start condition
    StartI2C1();
    ok = i2c_master_wait_and_check(I2C_MOD1, 0);   //Wait op. ends and check errors
    if(!ok) return count;                       //Return in error

    //Send device address
    MasterWriteI2C1(w_address);
    ok = i2c_master_wait_and_check(I2C_MOD1, 1);
    if(!ok) return count;

    //Send byte address
    for(count=1; count<addlen; count++)
    {
        MasterWriteI2C1(address[count]);
        ok = i2c_master_wait_and_check(I2C_MOD1, 1);
        if(!ok) return 0;
    }

    //STEP2 -> Start reception
    RestartI2C1();
    ok = i2c_master_wait_and_check(I2C_MOD1, 0);
    if(!ok) return 0;

    MasterWriteI2C1(r_address); //Address + W
    ok = i2c_master_wait_and_check(I2C_MOD1, 1);
    if(!ok) return 0;

    register int last = len-1;
    for(count=0; count<len; count++)
    {
        I2C1CONbits.RCEN = 1; //Start reception
        ok = i2c_master_wait_and_check(I2C_MOD1, 0); //Wait 8 clocks
        if(!ok) return count;

        data[count] = (char)I2C1RCV; //Read received byte
        I2C1STATbits.I2COV = 0; //Clear overflow flag

        if(count < last) AckI2C1(); //Send ACK
        else NotAckI2C1(); //Not ACK and ends operation

        ok = i2c_master_wait_and_check(I2C_MOD1, 0); //Wait ack ends
        if(!ok) return count;
    }

    //Close session sending stop condition
    StopI2C1();
    i2c_master_wait_and_check(I2C_MOD1, 0);

    return count;
}

/**
 * Master I2C1 interrupt
 */
void __attribute__((__interrupt__, auto_psv)) _MI2C1Interrupt(void)
{
    MI2C1_Clear_Intr_Status_Bit;

#ifdef _I2C_RTOS_WAIT
#endif

#ifdef _I2C_PWRS_WAIT
#endif

#ifdef _I2C_BUSY_WAIT
#endif
}

/**
 * Wait for I2C interrupt cheking if was caused by a bus collision. If @check_ack
 * is set then also checks for ACK. A stop condition is generated on error.
 *
 * @param device I2C module to use
 * @param check_ack Indicates if ACK status bit must be read. Set to 0 when
 * the function is called after a START, RESTART, STOP or READ operation.
 *
 * @return 1 Ok, 0 Error.
 */
static int i2c_master_wait_and_check(int device, int check_ack)
{
    int error;
    i2c_master_wait(device);                  //Wait for stage completion
    error = i2c_check_bus_collision(device);  //Check if bus collision occurred

    if(check_ack) error = error || (!i2c_check_ack(device));   //Check ACK
    
    if(error)                                   //Send stop condition in error
    {
        switch(device)                          //Stop condition
        {
            case I2C_MOD1:
                StopI2C1();
                break;
            case I2C_MOD2:
                StopI2C2();
                break;
            case I2C_MOD3:
                StopI2C3();
                break;
        }
        
        i2c_master_wait(device);
        return 0;                               //In error return 0
    }
    
    return 1;                                   //If not return 1
}

/**
 * Check slave ACK status.
 *
 * @param device I2C module to use
 * @return 1 ACK, 0 Not ACK.
 */
static int i2c_check_ack(int device)
{
    int n_ack; //I2C module uses negative logic for ACK

    switch (device)
    {
        case I2C_MOD1:
            n_ack = I2C1STATbits.ACKSTAT;
            break;
        case I2C_MOD2:
            n_ack = I2C2STATbits.ACKSTAT;
            break;
        case I2C_MOD3:
            n_ack = I2C3STATbits.ACKSTAT;
            break;
        default:
            n_ack = 1;
            break;
    }

    return !n_ack; //Return using positive logic
}

/**
 * Wait for master operations ends, ie, interrupt generation.
 *
 * Depending of the implementation, can use two methods:
 *     * Polling: Interrupt are disabled so perform a busy waiting polling
 *       MI2CxIF flag
 *
 *     * Power save: CPU enters in IDLE mode until I2C ISR is generated.
 * 
 *     * FreeRTOS ISR: Uses interrupt service routine and semaphore sync with
 *       FreeRTOS
 */
static void i2c_master_wait(int device)
{
#ifdef _I2C_RTOS_WAIT
#endif

#ifdef _I2C_PWRS_WAIT
{
    switch (device)
    {
        case I2C_MOD1:
            while(0 == IFS1bits.MI2C1IF){Idle();}
            IFS1bits.MI2C1IF = 0;
            break;
        case I2C_MOD2:
            while(0 == IFS3bits.MI2C2IF){Idle();}
            IFS3bits.MI2C2IF = 0;
            break;
        case I2C_MOD3:
            while(0 == IFS5bits.MI2C3IF){Idle();}
            IFS5bits.MI2C3IF = 0;
            break;
        default:
            break;
    }

    return;
}
#endif
    
#ifdef _I2C_BUSY_WAIT
{
    switch (device)
    {
        case I2C_MOD1:
            MasterWaitForIntrI2C1();
            break;
        case I2C_MOD2:
            MasterWaitForIntrI2C2();
            break;
        case I2C_MOD3:
            MasterWaitForIntrI2C3();
            break;
        default:
            break;
    }
    
    return;
}
#endif
}

/**
 * Checks if bus collision ocurrs, clear status and return.
 *
 * @param device I2C module to use
 * @return 1 collision, 0 not collision
 */
static int i2c_check_bus_collision(int device)
{
    int iwcol_flag;

    //Reads and clear IWCOL status bit
    switch(device)
    {
        case I2C_MOD1:
            iwcol_flag = I2C1STATbits.IWCOL;
            I2C1STATbits.IWCOL = 0;
            break;
        case I2C_MOD2:
            iwcol_flag = I2C2STATbits.IWCOL;
            I2C2STATbits.IWCOL = 0;
            break;
        case I2C_MOD3:
            iwcol_flag = I2C3STATbits.IWCOL;
            I2C3STATbits.IWCOL = 0;
            break;
        default:
            break;
    }

    return iwcol_flag;
}

/**
 * Slave I2C1 interrupt
 */
void __attribute__((__interrupt__, auto_psv)) _SI2C1Interrupt(void)
{
    SI2C1_Clear_Intr_Status_Bit;

    I2C1STATbits.I2COV = 0;
    char data = (char)I2C1RCV;

    if(I2C1STATbits.D_A == 0)
    {
        if(I2C1STATbits.R_W == 0) //Write
            I2C_SLAVE_ST = I2C_SLV_ADDR;
        else
            I2C_SLAVE_ST = I2C_SLV_READ;
        
        I2C1CONbits.SCLREL = 1;
        return;
    }

    switch(I2C_SLAVE_ST)
    {
        case I2C_SLV_ADDR:
            i2c_slave_address = data;
            I2C_SLAVE_ST = I2C_SLV_DATA;
            break;
        case I2C_SLV_DATA:
            i2c_slave_putc(data);
            i2c_slave_address++;
            break;
        case I2C_SLV_READ:
            I2C1TRN = (unsigned int)i2c_slave_getc();
            i2c_slave_address++;
            break;
        default:
            break;
    }

    I2C1CONbits.SCLREL = 1;
}

/**
 * Write a byte into slave device at @i2c_slave_address
 * @param data Byte to be write
 */
static void i2c_slave_putc(char data)
{
#ifdef _I2C_SLAVE_RTOS
#endif
    
#ifdef _I2C_SLAVE_BUFF
    if(i2c_slave_address < _I2C_RCV_BUFF_LEN)
        I2C_BUFF[i2c_slave_address] = data;
#endif
}

/**
 * Reads a byte from slave device
 * @return stored byte at @i2c_slave_address
 */
static char i2c_slave_getc(void)
{
#ifdef _I2C_SLAVE_RTOS
#endif

#ifdef _I2C_SLAVE_BUFF
    if(i2c_slave_address < _I2C_RCV_BUFF_LEN)
        return I2C_BUFF[i2c_slave_address];
    else
        return 0;
#endif
}

