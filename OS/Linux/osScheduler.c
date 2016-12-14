/**
 * @file  Linux/scheuler.c
 * @author Ignacio Ibañez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related with scheuler for systems operating Linux
 *
 */

#include "../include/osScheduler.h"

/**
 * starts the scheduler of the system operating
 */
void osScheduler(){
    printf(">>Starting Linux scheduler [->]\r\n");

    /* FIXME: Whait other thread to finish */
    while(1){}
    
    /* FIXME: In case of error the application should be closed */
    /* FIXME: Catch term or exit or kill signal to do a clean exit */
    while(1){
    	printf("\n>>Linux [FAIL]\n");
    }
}
