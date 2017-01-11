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
void osScheduler(os_thread** thread_id, int n_thread){
    printf(">>Starting Linux scheduler [->]\r\n");

    int i;
    for(i = 0; i < n_thread; i++){
        pthread_join(*thread_id[i], NULL);
    }
    
    /* FIXME: In case of error the application should be closed */
    /* FIXME: Catch term or exit or kill signal to do a clean exit */
    /*for(i = 0; i < n_thread; i++){
        free(*thread_id[i]);
    }*/
    exit(0);
}
