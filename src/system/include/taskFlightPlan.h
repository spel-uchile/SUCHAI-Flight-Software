//
// Created by grynn on 12-12-17.
//

#ifndef SUCHAI_FLIGHT_SOFTWARE_TASKFLIGHTPLAN_H
#define SUCHAI_FLIGHT_SOFTWARE_TASKFLIGHTPLAN_H

#endif //SUCHAI_FLIGHT_SOFTWARE_TASKFLIGHTPLAN_H

#include "data_storage.h"
#include <utils.h>
#include <stdlib.h>
#include "osDelay.h"
#include "repoCommand.h"
#include "repoData.h"

void taskFlightPlan(void *param);
int date_to_unixtime(int day, int month, int year, int hour, int min, int sec);