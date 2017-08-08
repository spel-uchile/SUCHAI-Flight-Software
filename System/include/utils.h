/**
 * @file utils.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2017
 * @copyright GNU GPL v3
 *
 * This header have definitions related with general utilities such as logging,
 * time formatting, etc.
 */

#ifndef UTILS_H
#define UTILS_H


#include "SUCHAI_config.h"
#include <time.h>
#include <pthread.h>

/**
 * @brief Log level
 *
 */
typedef enum {
    LOG_LVL_NONE,       /*!< No log output */
    LOG_LVL_ERROR,      /*!< Critical errors, software module can not recover on its own */
    LOG_LVL_WARN,       /*!< Error conditions from which recovery measures have been taken */
    LOG_LVL_INFO,       /*!< Information messages which describe normal flow of events */
    LOG_LVL_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    LOG_LVL_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} log_level_t;

// Define default log level
#ifndef LOG_LEVEL
#define LOG_LEVEL ((log_level_t)LOG_LVL_DEBUG)
#endif

#define LF   "\n"
#define CRLF "\r\n"

// Logging functions
pthread_mutex_t print_mutex;

#define LOGE(tag, msg, ...) if(LOG_LEVEL >= LOG_LVL_ERROR)   {pthread_mutex_lock(&print_mutex); printf("[ERROR][%lu][%s] ", (unsigned long)time(NULL), tag); printf(msg, ##__VA_ARGS__); printf(LF); fflush(stdout); pthread_mutex_unlock(&print_mutex);}
#define LOGW(tag, msg, ...) if(LOG_LEVEL >= LOG_LVL_WARN)    {pthread_mutex_lock(&print_mutex); printf("[WARN ][%lu][%s] ", (unsigned long)time(NULL), tag); printf(msg, ##__VA_ARGS__); printf(LF); fflush(stdout); pthread_mutex_unlock(&print_mutex);}
#define LOGI(tag, msg, ...) if(LOG_LEVEL >= LOG_LVL_INFO)    {pthread_mutex_lock(&print_mutex); printf("[INFO ][%lu][%s] ", (unsigned long)time(NULL), tag); printf(msg, ##__VA_ARGS__); printf(LF); fflush(stdout); pthread_mutex_unlock(&print_mutex);}
#define LOGD(tag, msg, ...) if(LOG_LEVEL >= LOG_LVL_DEBUG)   {pthread_mutex_lock(&print_mutex); printf("[DEBUG][%lu][%s] ", (unsigned long)time(NULL), tag); printf(msg, ##__VA_ARGS__); printf(LF); fflush(stdout); pthread_mutex_unlock(&print_mutex);}
#define LOGV(tag, msg, ...) if(LOG_LEVEL >= LOG_LVL_VERBOSE) {pthread_mutex_lock(&print_mutex); printf("[VERB ][%lu][%s] ", (unsigned long)time(NULL), tag); printf(msg, ##__VA_ARGS__); printf(LF); fflush(stdout); pthread_mutex_unlock(&print_mutex);}

// Assert functions
#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define log_error(T, M, ...) LOGE(T, "(%s:%d: errno: %s) " M, __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define assertf(A, T, M, ...) if(!(A)) {log_error(T, M, ##__VA_ARGS__); assert(A); }

#endif //UTILS_H
