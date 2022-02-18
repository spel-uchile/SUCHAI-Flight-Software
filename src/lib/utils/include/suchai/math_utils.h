/**
 * @file math_utils.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @author Elias Obreque S - elias.obreque@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header have definitions related mathematical utilities such as matrices, vectors or quaternions,
 * time formatting, etc.
 */

#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "suchai/config.h"

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
 * Calculates unit quaternion from a pure vector.
 * @param axis vector of dimension 3.
 * @param res equivalent quaternion.
 */
void vec_to_quat(vector3_t axis, quaternion_t * res);

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
 * Quaternion to rotation matrix transformation
 * @param q Input quaternion
 * @param res 3X3 result transformation matrix
 */
void quat_to_dcm(quaternion_t * q, matrix3_t * res);

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
void mat_vec_mult(matrix3_t mat, vector3_t vec, vector3_t * res);

/**
 * Calculates (3x3) matrix product:
 * res = rhs * lhs
 * @param lhs left hand (3x3) matrix.
 * @param rhs right hand (3x3) matrix.
 * @param res matrix multiplication result (3x3).
 */
void mat_mat_mult(matrix3_t lhs, matrix3_t rhs, matrix3_t* res);

/**
 * Calculates (3x3) matrix sum:
 * res = rhs + lhs
 * @param lhs left hand (3x3) matrix.
 * @param rhs right hand (3x3) matrix.
 * @param res matrix sum result (3x3).
 */
void mat_mat_sum(matrix3_t lhs, matrix3_t rhs, matrix3_t* res);

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

/**
 * Calculates skew matrix from a 3 dimension vector
 * @param vec input vector
 * @param res (3x3) result matrix
 */
void mat_skew(vector3_t vec, matrix3_t* res);

/**
 * Calculates matrix inverse for a (3x3) matrix
 * @param mat Input (3x3) matrix
 * @param res Inverse matrix result.
 */
void mat_inverse(matrix3_t mat, matrix3_t* res);

/**
 * Calculates matrix transpose for a (3x3) matrix
 * @param mat Input (3x3) matrix.
 * @param res Transpose matrix result.
 */
void mat_transpose(matrix3_t* mat, matrix3_t* res);

quaternion_t quat_from_dcm(matrix3_t dcm);

double abs_double(double x);

void _mat_cons_mult(double  a, double * mat, double *res, int n_x, int n_y);

void _mat_vec_mult(double * mat, double * vec, double * res, int n_x, int n_y);

void _mat_mat_mult(double * lhs, double * rhs, double * res, int n_x, int n_y, int n_z);

void _mat_mat_sum(double * lhs, double * rhs, double * res, int n_x, int n_y);

void _mat_set_diag(double * m, double val, int n_x, int n_y);

void _mat_transpose(double * mat, double * res, int n_i, int n_j);

void _mat_copy(double * mat, double * res, int matx, int maty, int resx, int resy, int p_i, int p_j);

void eskf_integrate(quaternion_t q, vector3_t omega, double dt, quaternion_t * res);

void eskf_compute_error(vector3_t omega, double dt, double P[6][6], double Q[6][6]);

void eskf_update_mag(vector3_t mag_sensor, vector3_t mag_i, double P[6][6], matrix3_t * R, quaternion_t * q, vector3_t * wb);

#endif //MATH_UTILS_H
