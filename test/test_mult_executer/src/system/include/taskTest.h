//
// Created by matias on 09-06-18.
//

#ifndef SUCHAI_FLIGHT_SOFTWARE_TASKTEST_H
#define SUCHAI_FLIGHT_SOFTWARE_TASKTEST_H

#include "repoCommand.h"
#include "osDelay.h"

/**
 * This function create some commands and send it to be executed
 */
void taskTest(void *param);

/**
 * Sleep the thread for s_sleep time
 *
 * @param s_sleep
 * @param actual_time
 * @return 0 if everything was ok
 */
int test_sleep(int s_sleep, int actual_time);

#endif //SUCHAI_FLIGHT_SOFTWARE_TASKTEST_H
