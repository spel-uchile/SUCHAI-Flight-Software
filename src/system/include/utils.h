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

#include "csp/csp.h"

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

extern osSemaphore log_mutex;  ///< Sync logging functions, require initialization

/**
 * Init logging system, specifically shared mutex
 * Set the log level and node to send logs. If node = -1, then print to stdout,
 * else send logs to another node using CSP
 * @param level Log level
 * @param node CSP node to send logs. Set to -1 to use stdout.
 * @return Int. CSP_SEMAPHORE_OK(1) or CSP_SEMAPHORE_ERROR(2)
 */
int log_init(log_level_t level, int node);

/**
 * Set the log level and node to send logs. If node = -1, then print to stdout,
 * else send logs to another node using CSP
 * @param level Log level
 * @param node CSP node to send logs. Set to -1 to use stdout.
 */
void log_set(log_level_t level, int node);

void log_print(const char *lvl, const char *tag, const char *msg, ...);
void log_send(const char *lvl, const char *tag, const char *msg, ...);

extern void (*log_function)(const char *lvl, const char *tag, const char *msg, ...);
extern log_level_t log_lvl;
extern uint8_t log_node;

/// Logging functions @see log_level_t
#define LOGE(tag, msg, ...) if(log_lvl >= LOG_LVL_ERROR)   {osSemaphoreTake(&log_mutex, portMAX_DELAY); log_function("ERROR", tag, msg, ##__VA_ARGS__); osSemaphoreGiven(&log_mutex);}
#define LOGW(tag, msg, ...) if(log_lvl >= LOG_LVL_WARN)    {osSemaphoreTake(&log_mutex, portMAX_DELAY); log_function("WARN ", tag, msg, ##__VA_ARGS__); osSemaphoreGiven(&log_mutex);}
#define LOGI(tag, msg, ...) if(log_lvl >= LOG_LVL_INFO)    {osSemaphoreTake(&log_mutex, portMAX_DELAY); log_function("INFO ", tag, msg, ##__VA_ARGS__); osSemaphoreGiven(&log_mutex);}
#define LOGD(tag, msg, ...) if(log_lvl >= LOG_LVL_DEBUG)   {osSemaphoreTake(&log_mutex, portMAX_DELAY); log_function("DEBUG", tag, msg, ##__VA_ARGS__); osSemaphoreGiven(&log_mutex);}
#define LOGV(tag, msg, ...) if(log_lvl >= LOG_LVL_VERBOSE) {osSemaphoreTake(&log_mutex, portMAX_DELAY); log_function("VERB ", tag, msg, ##__VA_ARGS__); osSemaphoreGiven(&log_mutex);}
#define LOGR(tag, msg, ...)                                {osSemaphoreTake(&log_mutex, portMAX_DELAY); log_print   ("REMOT", tag, msg, ##__VA_ARGS__); osSemaphoreGiven(&log_mutex);}

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
