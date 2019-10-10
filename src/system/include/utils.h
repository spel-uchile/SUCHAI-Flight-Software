/**
 * @file utils.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2019
 * @copyright GNU GPL v3
 *
 * This header have definitions related with general utilities such as logging,
 * time formatting, etc.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

#include "config.h"
#include "os/os.h"
#include "osSemphr.h"

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

#define LOGOUT stdout   ///<! Log to stdout
//#define LOGOUT stderr   ///<! Log to stderr

#define LF   "\n"       ///< Use LF terminated log strings
#define CRLF "\r\n"     ///< USE CRLF terminated log strings

osSemaphore log_mutex;  ///< Sync logging functions, require initialization

/**
 * Init logging system, specifically shared mutex
 * @return Int. CSP_SEMAPHORE_OK(1) or CSP_SEMAPHORE_ERROR(2)
 */
static inline int log_init(void)
{
    return osSemaphoreCreate(&log_mutex);
}

/// Logging functions @see log_level_t
#define LOGE(tag, msg, ...) if(LOG_LEVEL >= LOG_LVL_ERROR)   {osSemaphoreTake(&log_mutex, portMAX_DELAY); fprintf(LOGOUT,"[ERROR][%lu][%s] ", (unsigned long)time(NULL), tag); fprintf(LOGOUT,msg, ##__VA_ARGS__); fprintf(LOGOUT,LF); fflush(LOGOUT); osSemaphoreGiven(&log_mutex);}
#define LOGW(tag, msg, ...) if(LOG_LEVEL >= LOG_LVL_WARN)    {osSemaphoreTake(&log_mutex, portMAX_DELAY); fprintf(LOGOUT,"[WARN ][%lu][%s] ", (unsigned long)time(NULL), tag); fprintf(LOGOUT,msg, ##__VA_ARGS__); fprintf(LOGOUT,LF); fflush(LOGOUT); osSemaphoreGiven(&log_mutex);}
#define LOGI(tag, msg, ...) if(LOG_LEVEL >= LOG_LVL_INFO)    {osSemaphoreTake(&log_mutex, portMAX_DELAY); fprintf(LOGOUT,"[INFO ][%lu][%s] ", (unsigned long)time(NULL), tag); fprintf(LOGOUT,msg, ##__VA_ARGS__); fprintf(LOGOUT,LF); fflush(LOGOUT); osSemaphoreGiven(&log_mutex);}
#define LOGD(tag, msg, ...) if(LOG_LEVEL >= LOG_LVL_DEBUG)   {osSemaphoreTake(&log_mutex, portMAX_DELAY); fprintf(LOGOUT,"[DEBUG][%lu][%s] ", (unsigned long)time(NULL), tag); fprintf(LOGOUT,msg, ##__VA_ARGS__); fprintf(LOGOUT,LF); fflush(LOGOUT); osSemaphoreGiven(&log_mutex);}
#define LOGV(tag, msg, ...) if(LOG_LEVEL >= LOG_LVL_VERBOSE) {osSemaphoreTake(&log_mutex, portMAX_DELAY); fprintf(LOGOUT,"[VERB ][%lu][%s] ", (unsigned long)time(NULL), tag); fprintf(LOGOUT,msg, ##__VA_ARGS__); fprintf(LOGOUT,LF); fflush(LOGOUT); osSemaphoreGiven(&log_mutex);}

/// Assert functions
#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define _log_error(T, M, ...) LOGE(T, "(%s:%d: errno: %s) " M, __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define assertf(A, T, M, ...) if(!(A)) {_log_error(T, M, ##__VA_ARGS__); assert(A); }

/// Debug buffer content
#define print_buff(buf, size) {int i; printf("["); for(i=0; i<size; i++) printf("0x%02X, ", buf[i]); printf("]\n");}
#define print_buff16(buf16, size) {int i; printf("["); for(i=0; i<size; i++) printf("0x%04X, ", buf16[i]); printf("]\n");}

/// Defines to string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#endif //UTILS_H
