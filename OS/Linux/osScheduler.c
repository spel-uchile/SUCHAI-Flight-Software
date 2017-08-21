/**
 * @file  Linux/scheuler.c
 * @author Ignacio Ibañez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related with scheuler for systems operating Linux
 *
 */

#include "osScheduler.h"

const static char *tag = "osScheduler";

/**
 * starts the scheduler of the system operating
 */
void osScheduler(os_thread* threads_id, int n_threads)
{
    LOGI(tag, "Linux scheduler: waiting threads")

    int i;
    for(i = 0; i < n_threads; i++){
        pthread_join(threads_id[i], NULL);
    }
    
    /* FIXME: In case of error the application should be closed */
    /* FIXME: Catch term or exit or kill signal to do a clean exit */
    exit(0);
}
