/**
 * @file utils.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Camilo Roja M - camrojas@uchile.cl
 * @author Elias Obreque S - elias.obreque@uchile.cl
 * @date 2020
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
#include <math.h>

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

/**
 * Quaternion structure
 * q = q0*i + q1*j + q2*k + q3
 * q = (scalar, vect)
 */
typedef union quaternion {
    double q[4];      ///< Quaternion as array
    struct {
          double q0; ///< Quaternion i
          double q1; ///< Quaternion j
          double q2; ///< Quaternion k
          double q3; ///< Quaternion scalar
      };
    struct {
      double vec[3]; ///< Quaternion vector
      double scalar;  ///< Quaternion scalar
    };
}quaternion_t;

/**
 * 3D vector structure
 */
typedef union vector3 {
    double v[3];    ///< Vector as array [x, y, z]
    struct {
        double v0;  ///< Vector x
        double v1;  ///< Vector y
        double v2;  ///< Vector z
    };
}vector3_t;

/**
 * 3x3 matrix structure
 */
typedef union matrix3 {
    double m[3][3];    ///< Matrix as array
    struct {
        double row0[3];
        double row1[3];
        double row2[3];
    };
}matrix3_t;

/**
 * Calculates equivalent quaternion from a rotation arround an axis.
 * @param axis vector of dimension 3 representing a rotation axis.
 * @param rot angle of rotation, in radians.
 * @param res equivalent rotation quaternion.
 */
void axis_rotation_to_quat(vector3_t axis, double rot, quaternion_t * res);

/**
 * Sum. of quaternions
 * @param q1 left side quaternion
 * @param q2 right side quaternion
 * @param res result quaternion
 */
void quat_sum(quaternion_t *q1, quaternion_t *q2, quaternion_t *res);

/**
 * Calculate the multiplication of two quaternions (it is not commutative)
 * @param lhs left side quaternion
 * @param rhs right side quaternion
 * @param res result of quaternion multiplication
 */
void quat_mult(quaternion_t *lhs, quaternion_t *rhs, quaternion_t *res);

/**
 * Normalize the quaternion
 * @param q quaternion
 * @param res quaternion normalized, if NULL result is stored in q
 */
void quat_normalize(quaternion_t *q, quaternion_t *res);

/**
 * Calculate the conjugate of the quaternion
 * @param q quaternion
 * @param res Conjugate of the quaternion
 */
void quat_conjugate(quaternion_t *q, quaternion_t *res);

/**
 * Transformation of vector v from (a) frame (_a) to (b) frame (_b)
 * @param q_rot_a2b Quaternion to rotate from reference (a) to reference frame (b)
 * @param v_a vector respect to (a) frame
 * @param v_b vector respect to (b) frame
 */
void quat_frame_conv(quaternion_t *q_rot_a2b, vector3_t *v_a, vector3_t *v_b);

/**
 * Calculate the inverse of the quaternion q
 * @param q quaternion
 * @param res inverse of quaternion
 */
void quat_inverse(quaternion_t *q, quaternion_t *res);

/**
 * Calculate euclidean norm in a 3 dimension vector
 * @param vec input vector
 * @return euclidean norm of vec
 */
double vec_norm(vector3_t vec);

/**
 * Normalize input vector vec into an unitary vector with norm 1
 * @param vec input vector of dim 3
 * @param res normalized unitary vector
 * @return 1 if it is ok, 0 if vec degenerates to a point
 */
int vec_normalize(vector3_t *vec, vector3_t *res);

/**
 * Calculates the inner product between two vectors of dimension 3
 * @param lhs left input vector of dim 3
 * @param rhs right input vector of dim 3
 * @return double with the inner product
 */
double vec_inner_product(vector3_t lhs, vector3_t rhs);

/**
 * Calculates the other product between two vectors of dimension 3,
 * by definition, the result vector will be perpendicular to input vectors.
 * @param lhs left input vector of dimension 3.
 * @param rhs right input vector of dimension 3.
 * @param res vector of dimension 3 , result of the inner product.
 */
void vec_outer_product(vector3_t lhs, vector3_t rhs, vector3_t * res);

/**
 * Calculates the angle, radians, between two input vectors of dim 3.
 * @param v1 first input vector of dim 3.
 * @param v2 second input vector of dim 3.
 * @return
 */
double vec_angle(vector3_t v1, vector3_t v2);

/**
 * Calculates the algebraic sum between two input vector of dim 3.
 * @param lhs left input vector of dim 3.
 * @param rhs right input vector of dim 3.
 * @param res vector storing the sum between inputs.
 */
void vec_sum(vector3_t lhs, vector3_t rhs, vector3_t * res);

/**
 * Multiply a vector of dim 3 with a constant.
 * res = a * vec.
 * @param a input constant.
 * @param vec input vector.
 * @param res vector of dimension 3. If NULL, result is stored in vec
 */
void vec_cons_mult(double a, vector3_t *vec, vector3_t *res);

/**
 * Calculates the matrix, vector product of dimensions (3,3) and 3.
 * w = M * v, where M is a (3x3) matrix, v is the input vector and w the result.
 * @param mat input matrix of dimensions (3x3).
 * @param vec input vector of dimensions
 * @param res
 */
void mat3_vec3_mult(matrix3_t mat, vector3_t vec, vector3_t * res);

/**
 * Set a diagonal 3x3 matrix
 *     [a 0 0]
 *     [0 b 0]
 *     [0 0 c]
 *
 * @param m pointer to matrix
 * @param a diagonal value
 * @param b diagonal value
 * @param c diagonal value
 */
void mat_set_diag(matrix3_t *m, double a, double b, double c);

#endif //UTILS_H
