/**
 * @file log_utils.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2024
 * @copyright GNU GPL v3
 *
 * This header have definitions related with logging to a mongo db
 */

#ifndef LOG_UTILS_MONGODB_H
#define LOG_UTILS_MONGODB_H

#include <assert.h>
#include <mongoc/mongoc.h>
#include <bson/bson.h>

#include "suchai/config.h"


typedef struct mongo_log_driver{
    mongoc_client_t *client;
} mongo_log_driver_t;

static mongo_log_driver_t mongo_log_driver = {.client=NULL};

/**
 * Initialize mongodb
 * @return 0 if Ok, -1 otherwise.
 */
int mongodb_log_init(void);

/**
 *
 * @param lvl
 * @param tag
 * @param msg
 * @param ...
 * @return
 */
int mongodb_log(const char *lvl, const char *tag, const char *msg, ...);

#endif //LOG_UTILS_MONGODB_H
