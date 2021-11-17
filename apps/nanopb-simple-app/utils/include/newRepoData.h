//
// Created by franco on 17-11-21.
//

#ifndef SUCHAI_FLIGHT_SOFTWARE_NEWREPODATA_H
#define SUCHAI_FLIGHT_SOFTWARE_NEWREPODATA_H

#endif //SUCHAI_FLIGHT_SOFTWARE_NEWREPODATA_H

#include "simple.pb.h"
#include "suchai/storage.h"

/**
 * Get data from "simple message" pesistent variable
 * @param message simple_message to get the data
 * @return 0 OK, -1 ERROR
 */
int get_simple_message(simple_message *message);

/**
 * Set data from "simple message" to save into any storage
 * @param message simple_message to set the data
 * @return 0 OK, -1 ERROR
 */
int set_simple_message(simple_message *message);