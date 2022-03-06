
#include "suchai/math_utils.h"

const double std_rw_w = 0.001;
const double std_rn_w = 0.001;
const double std_rn_mag = 0.001;

void quat_sum(quaternion_t *q1, quaternion_t *q2, quaternion_t *res)
{
    int i;
    for(i = 0; i<4; i++)
        res->q[i] = q1->q[i] + q2->q[i];
}

void quat_mult(quaternion_t *lhs, quaternion_t *rhs, quaternion_t *res) {
    // i, j, k, 1
    res->q[0] = lhs->q[3]*rhs->q[0]-lhs->q[2]*rhs->q[1]+lhs->q[1]*rhs->q[2]+lhs->q[0]*rhs->q[3];
    res->q[1] = lhs->q[2]*rhs->q[0]+lhs->q[3]*rhs->q[1]-lhs->q[0]*rhs->q[2]+lhs->q[1]*rhs->q[3];
    res->q[2] =-lhs->q[1]*rhs->q[0]+lhs->q[0]*rhs->q[1]+lhs->q[3]*rhs->q[2]+lhs->q[2]*rhs->q[3];
    res->q[3] =-lhs->q[0]*rhs->q[0]-lhs->q[1]*rhs->q[1]-lhs->q[2]*rhs->q[2]+lhs->q[3]*rhs->q[3];
}

void quat_normalize(quaternion_t *q, quaternion_t *res)
{
    double n = 0.0;
    for(int i=0; i<4 ; i++)
        n += pow(q->q[i], 2.0);

    if (n == 0.0)
        return;

    n = 1.0/sqrt(n);
    for(int i=0; i<4; i++)
        if(res != NULL)
            res->q[i] = q->q[i]*n;
        else
            q->q[i] = q->q[i]*n;
}

void quat_inverse(quaternion_t *q, quaternion_t *res)
{
    quaternion_t temp1;
    quat_normalize(q, &temp1);
    quat_conjugate(&temp1, res);
}

void quat_conjugate(quaternion_t *q, quaternion_t *res)
{
    // i, j, k, 1
    res->q[0] = -q->q[0];
    res->q[1] = -q->q[1];
    res->q[2] = -q->q[2];
    res->q[3] = q->q[3];
}

void quat_frame_conv(quaternion_t *q_rot_a2b, vector3_t *v_a, vector3_t *v_b)
{
    quaternion_t q_conj;
    quat_conjugate(q_rot_a2b, &q_conj);
    quaternion_t vec_as_quat;
    vec_as_quat.q[0] = v_a->v[0];
    vec_as_quat.q[1] = v_a->v[1];
    vec_as_quat.q[2] = v_a->v[2];
    vec_as_quat.q[3] = 0;

    quaternion_t temp;
    quat_mult(&q_conj, &vec_as_quat, &temp);
    quaternion_t res_vec;
    quat_mult(&temp, q_rot_a2b, &res_vec);

    v_b->v[0] = res_vec.q[0];
    v_b->v[1] = res_vec.q[1];
    v_b->v[2] = res_vec.q[2];
    //double q0 = q_rot_a2b->q3; // real part
    //double q1 = q_rot_a2b->q0; // i
    //double q2 = q_rot_a2b->q1; // j
    //double q3 = q_rot_a2b->q2; // k

    //v_b->v[0] = (2.0 * pow(q0, 2.0) - 1.0 + 2.0 * pow(q1, 2.0)) * v_a->v[0] + (2 * q1 * q2 + 2.0 * q0 * q3) * v_a->v[1] + (2.0 * q1 * q3 - 2.0 * q0 * q2) * v_a->v[2];
    //v_b->v[1] = (2.0 * q1 * q2 - 2.0 * q0 * q3) * v_a->v[0] + (2 * pow(q2, 2.0) + 2.0 * pow(q0, 2.0) - 1.0) * v_a->v[1] + (2.0 * q2 * q3 + 2.0 * q0 * q1) * v_a->v[2];
    //v_b->v[2] = (2.0 * q1 * q3 + 2.0 * q0 * q2) * v_a->v[0] + (2 * q2 * q3 - 2.0 * q0 * q1) * v_a->v[1] + (2.0 * pow(q3, 2.0) + 2.0 * pow(q0, 2.0) - 1.0) * v_a->v[2];
}

void quat_to_dcm(quaternion_t * q, matrix3_t * res)
{
    double q1 = q->q[0];
    double q2 = q->q[1];
    double q3 = q->q[2];
    double q4 = q->q[3];

    res->m[0][0] = pow(q1, 2) - pow(q2, 2) - pow(q3, 2) + pow(q4, 2);
    res->m[0][1] = 2 * (q1 * q2 + q3 * q4);
    res->m[0][2] = 2 * (q1 * q3 - q2 * q4);

    res->m[1][0] = 2 * (q1 * q2 - q3 * q4);
    res->m[1][1] = -1.0 * pow(q1, 2) + pow(q2, 2) - pow(q3, 2) + pow(q4, 2);
    res->m[1][2] =  2 * (q2 * q3 + q1 * q4);

    res->m[2][0] = 2 * (q1 * q3 + q2 * q4);
    res->m[2][1] = 2 * (q2 * q3 - q1 * q4);
    res->m[2][2] = -1.0 * pow(q1, 2) - pow(q2, 2) + pow(q3, 2) + pow(q4, 2);
}

void axis_rotation_to_quat(vector3_t axis, double rot, quaternion_t * res )
{
    rot *= 0.5;
    res->scalar = cos(rot);

    for(int i=0; i < 3; ++i) {
        res->vec[i] = axis.v[i] * sin(rot);
    }
}

void vec_to_quat(vector3_t axis, quaternion_t * res)
{
    double rot = vec_norm(axis);
    vector3_t u;
    vec_cons_mult(1.0 / rot, &axis, &u);
    axis_rotation_to_quat(u, rot, res);
}

double vec_norm(vector3_t vec)
{
    double res = 0.0;
    for(size_t i=0; i<3; ++i){
        res += pow(vec.v[i], 2.0);
    }
    return sqrt(res);
}

int vec_normalize(vector3_t *vec, vector3_t *res)
{
    double n = vec_norm(*vec);
    if(n == 0.0) { return 0; }

    n = 1.0/n;
    for(int i=0; i<3; ++i){
        if(res != NULL)
            res->v[i] = vec->v[i]*n;
        else
            vec->v[i]*=n;
    }
    return 1;
}

double vec_inner_product(vector3_t lhs, vector3_t rhs)
{
    double res = 0;
    for(int i=0; i<3; ++i) {
        res += lhs.v[i] * rhs.v[i];
    }
    return res;
}

void vec_outer_product(vector3_t lhs, vector3_t rhs, vector3_t * res)
{
    res->v[0] = lhs.v[1]*rhs.v[2]-lhs.v[2]*rhs.v[1];
    res->v[1] = lhs.v[2]*rhs.v[0]-lhs.v[0]*rhs.v[2];
    res->v[2] = lhs.v[0]*rhs.v[1]-lhs.v[1]*rhs.v[0];
}

double vec_angle(vector3_t v1, vector3_t v2)
{
    double cos = vec_inner_product(v1, v2) / (vec_norm(v1)*vec_norm(v2));
    return acos(cos);
}

void vec_sum(vector3_t lhs, vector3_t rhs, vector3_t * res)
{
    for(int i=0; i<3; ++i) {
        res->v[i] = lhs.v[i]+rhs.v[i];
    }
}

void vec_cons_mult(double a, vector3_t *vec, vector3_t *res)
{
    for(int i=0; i<3; ++i){
        if(res != NULL)
            res->v[i] = vec->v[i] * a;
        else
            vec->v[i] = vec->v[i] * a;
    }
}

void quat_cons_mult(double a, quaternion_t *vec, quaternion_t *res)
{
    for(int i=0; i<3; ++i){
        if(res != NULL)
            res->q[i] = vec->q[i] * a;
        else
            vec->q[i] = vec->q[i] * a;
    }
}

void mat_vec_mult(matrix3_t mat, vector3_t vec, vector3_t * res)
{
    _mat_vec_mult((double *) mat.m, (double *) vec.v, (double *) res->v,3, 3);
}

void mat_quat_mult(matrix4_t mat, quaternion_t vec, quaternion_t * res)
{
    _mat_vec_mult((double *) mat.m, (double *) vec.q, (double *) res->q,4, 4);
}

void mat_mat_mult(matrix3_t lhs, matrix3_t rhs, matrix3_t* res)
{
    _mat_mat_mult((double *) lhs.m, (double *) rhs.m, (double *) res->m, 3, 3, 3);
}

void mat_mat_sum(matrix3_t lhs, matrix3_t rhs, matrix3_t* res)
{
    _mat_mat_sum((double *) lhs.m, (double *) rhs.m, (double *) res->m, 3, 3);
}

void mat_set_diag(matrix3_t *m, double a, double b, double c)
{
    m->row0[0] = a; m->row0[1] = 0; m->row0[2] = 0;
    m->row1[0] = 0; m->row1[1] = b; m->row1[2] = 0;
    m->row2[0] = 0; m->row2[1] = 0; m->row2[2] = c;
}

void mat_set_diag4x(matrix4_t *m, double a, double b, double c, double d)
{
    m->row0[0] = a; m->row0[1] = 0; m->row0[2] = 0; m->row0[3] = 0;
    m->row1[0] = 0; m->row1[1] = b; m->row1[2] = 0; m->row1[3] = 0;
    m->row2[0] = 0; m->row2[1] = 0; m->row2[2] = c; m->row2[3] = 0;
    m->row3[0] = 0; m->row3[1] = 0; m->row3[2] = 0; m->row3[3] = d;
}

void mat_cons_mult(double a, matrix3_t * m, matrix3_t * res)
{
    for (int i=0; i<3; i++){
        for (int j=0; j<3; j++){
            if(res != NULL)
                res->m[i][j] = m->m[i][j] * a;
            else
                m->m[i][j] = m->m[i][j] * a;
        }
    }
}

void mat_transpose(matrix3_t* mat, matrix3_t* res)
{
    _mat_transpose((double *) mat->m, (double *) res->m, 3, 3);
}

void mat_skew(vector3_t vec, matrix3_t * res)
{
    res->m[1][0] = vec.v[2];
    res->m[2][0] = -1.0 * vec.v[1];

    res->m[0][1] = -1.0 * vec.v[2];
    res->m[0][2] = vec.v[1];

    res->m[2][1] = vec.v[0];
    res->m[1][2] = -1.0 * vec.v[0];
}

void mat_inverse(matrix3_t mat, matrix3_t* res)
{
    double a, b, c, d, e, f, g, h, i;
    a = mat.m[0][0]; b = mat.m[0][1], c = mat.m[0][2];
    d = mat.m[1][0]; e = mat.m[1][1], f = mat.m[1][2];
    g = mat.m[2][0]; h = mat.m[2][1], i = mat.m[2][2];

    double A = (e*i - f*h);
    double B = -1.0 * (d*i - f*g);
    double C = (d*h -e*g);
    double D = -1.0* (b*i - c*h);
    double E = (a*i - c*g);
    double F = -1.0 * (a*h -b*g);
    double G = (b*f -c*e);
    double H = -1.0 * (a*f - c*d);
    double I = (a*e -b*d);
    double detmat = a*A + b*B + c*C;

    assert(fabs(detmat) >= 1E-25);

    res->m[0][0] = A/detmat; res->m[0][1] = D/detmat, res->m[0][2] = G/detmat;
    res->m[1][0] = B/detmat; res->m[1][1] = E/detmat, res->m[1][2] = H/detmat;
    res->m[2][0] = C/detmat; res->m[2][1] = F/detmat, res->m[2][2] = I/detmat;
}

void calc_inverse_matrix(matrix7_t S_j, matrix7_t * S_j_i){
    calc_cofactor(S_j, S_j_i, 7);
}


// function for cofactor calculation
void calc_cofactor(matrix7_t lhs, matrix7_t * res, int f)
{
    matrix7_t b, fac;
    int p, q, m, n, i, j;
    for (q = 0;q < f; q++)
    {
        for (p = 0;p < f; p++)
        {
            m = 0;
            n = 0;
            for (i = 0;i < f; i++)
            {
                for (j = 0;j < f; j++)
                {
                    if (i != q && j != p)
                    {
                        b.m[m][n] = lhs.m[i][j];
                        if (n < (f - 2))
                            n++;
                        else
                        {
                            n = 0;
                            m++;
                        }
                    }
                }
            }
            fac.m[q][p] = pow(-1, q + p) * calc_determinant(b, f - 1);
        }
    }
    matrix7_t temp1;
    temp1 = calc_transpose(lhs, fac, f);
    res->m[0][0] = temp1.m[0][0]; res->m[0][1] = temp1.m[0][1]; res->m[0][2] = temp1.m[0][2]; res->m[0][3] = temp1.m[0][3]; res->m[0][4] = temp1.m[0][4]; res->m[0][5] = temp1.m[0][5]; res->m[0][6] = temp1.m[0][6];
    res->m[1][0] = temp1.m[1][0]; res->m[1][1] = temp1.m[1][1]; res->m[1][2] = temp1.m[1][2]; res->m[1][3] = temp1.m[1][3]; res->m[1][4] = temp1.m[1][4]; res->m[1][5] = temp1.m[1][5]; res->m[1][6] = temp1.m[1][6];
    res->m[2][0] = temp1.m[2][0]; res->m[2][1] = temp1.m[2][1]; res->m[2][2] = temp1.m[2][2]; res->m[2][3] = temp1.m[2][3]; res->m[2][4] = temp1.m[2][4]; res->m[2][5] = temp1.m[2][5]; res->m[2][6] = temp1.m[2][6];
    res->m[3][0] = temp1.m[3][0]; res->m[3][1] = temp1.m[3][1]; res->m[3][2] = temp1.m[3][2]; res->m[3][3] = temp1.m[3][3]; res->m[3][4] = temp1.m[3][4]; res->m[3][5] = temp1.m[3][5]; res->m[3][6] = temp1.m[3][6];
    res->m[4][0] = temp1.m[4][0]; res->m[4][1] = temp1.m[4][1]; res->m[4][2] = temp1.m[4][2]; res->m[4][3] = temp1.m[4][3]; res->m[4][4] = temp1.m[4][4]; res->m[4][5] = temp1.m[4][5]; res->m[4][6] = temp1.m[4][6];
    res->m[5][0] = temp1.m[5][0]; res->m[5][1] = temp1.m[5][1]; res->m[5][2] = temp1.m[5][2]; res->m[5][3] = temp1.m[5][3]; res->m[5][4] = temp1.m[5][4]; res->m[5][5] = temp1.m[5][5]; res->m[5][6] = temp1.m[5][6];
    res->m[6][0] = temp1.m[6][0]; res->m[6][1] = temp1.m[6][1]; res->m[6][2] = temp1.m[6][2]; res->m[6][3] = temp1.m[6][3]; res->m[6][4] = temp1.m[6][4]; res->m[6][5] = temp1.m[6][5]; res->m[6][6] = temp1.m[6][6];
}


///function to find the transpose of a matrix
matrix7_t calc_transpose(matrix7_t num, matrix7_t fac, int r)
{
    int i, j;
    matrix7_t b;
    double d;
    matrix7_t inverse;

    for (i = 0;i < r; i++)
    {
        for (j = 0;j < r; j++)
        {
            b.m[i][j] = fac.m[j][i];
        }
    }

    d = calc_determinant(num, r);
    for (i = 0;i < r; i++)
    {
        for (j = 0;j < r; j++)
        {
            inverse.m[i][j] = b.m[i][j] / d;
        }
    }
    return inverse;
}

// function for the calculation of determinant
double calc_determinant(matrix7_t a, int k)
{
    float s = 1;
    double det;
    matrix7_t b;
    int i, j, m, n, c;
    if (k == 1)
    {
        return (a.m[0][0]);
    }
    else
    {
        det = 0;
        for (c = 0; c < k; c++)
        {
            m = 0;
            n = 0;
            for (i = 0;i < k; i++)
            {
                for (j = 0 ;j < k; j++)
                {
                    b.m[i][j] = 0;
                    if (i != 0 && j != c)
                    {
                        b.m[m][n] = a.m[i][j];
                        if (n < (k - 2))
                            n++;
                        else
                        {
                            n = 0;
                            m++;
                        }
                    }
                }
            }
            det = det + s * (a.m[0][c] * calc_determinant(b, k - 1));
            s = -1 * s;
        }
    }
    return (det);
}

void _mat_cons_mult(double  a, double * mat, double *res, int n_x, int n_y)
{
    for(int i=0; i < n_x*n_y; ++i) {
        if(res != NULL)
            res[i] = mat[i] * a;
        else
            mat[i] = mat[i] * a;
    }
}

void _mat_vec_mult(double * mat, double * vec, double * res, int n_x, int n_y)
{
    int mati = 0;
    for(int i=0; i< n_x; ++i)
    {
        res[i] = 0.0;
        for(int j=0; j< n_y; ++j)
        {
            mati = (i * n_y) + j;
            res[i] += mat[mati]*vec[j];
        }
    }
}

void _mat_mat_mult(double * lhs, double * rhs, double * res, int n_x, int n_y, int n_z)
{
    int ires=0, il=0, ir=0;
    for(int i=0; i < n_x; ++i) {
        for(int k=0; k < n_z; ++k) {
            ires = (i*n_z) + k;
            res[ires] = 0.0;
            for(int j=0; j < n_y; ++j) {
                il = (i*n_y) + j;
                ir = (j*n_z) + k;
                res[ires] += lhs[il]*rhs[ir];
            }
        }
    }
}

void _mat_mat_sum(double * lhs, double * rhs, double * res, int n_x, int n_y)
{
    for(int i=0; i < n_x *n_y; ++i) {
        res[i] = lhs[i] + rhs[i];
    }
}

void _mat_transpose(double * mat, double * res, int n_x, int n_y)
{
    int index=0, index_t=0;
    for(int i=0; i < n_x; ++i) {
        for(int j=0; j < n_y; ++j) {
            index = (n_y*i) + j;
            index_t = (n_x*j) + i;
            res[index_t] = mat[index];
        }
    }
}

void _mat_copy(double * mat, double * res, int matx, int maty, int resx, int resy, int p_i, int p_j)
{
    int i_mat=0, i_res=0;
    for(int i=0; i < matx; ++i) {
        for(int j=0; j < maty; ++j) {
            i_mat = (matx*i) + j;
            i_res = ((resx)*(i+p_i)) + j + p_j;
            res[i_res] = mat[i_mat];
        }
    }
}

void _mat_set_diag(double * m, double val, int n_x, int n_y)
{
    int mi = 0;
    for(int i=0; i < n_x; ++i) {
        for(int j=0; j < n_y; ++j) {
            mi = (i*n_y) + j ;
            if (i == j)
                m[mi] = val;
            else
                m[mi] = 0.0;
        }
    }
}

void eskf_integrate(quaternion_t q, vector3_t omega, double dt, quaternion_t * res)
{
    vector3_t omega_dt;
    vec_cons_mult(dt, &omega, &omega_dt);
    quaternion_t q_omega_dt;
    vec_to_quat(omega_dt, &q_omega_dt);
    quat_mult(&q, &q_omega_dt, res);
}

void eskf_compute_error(vector3_t omega, double dt, double P[6][6], double Q[6][6])
{
//    matrix3_t  F[2][2];
    double  F[6][6];
    // F11
    vector3_t omega_dt;
    vec_cons_mult(dt, &omega, &omega_dt);
    quaternion_t dq_omegadt;
    vec_to_quat(omega_dt, &dq_omegadt);
    matrix3_t rwb, temp;
    quat_to_dcm(&dq_omegadt, &rwb);
    mat_transpose(&rwb, &temp);
    _mat_copy((double*) temp.m, (double*) F, 3, 3, 6, 6, 0, 0);
    // F12
    double diag_val = -1.0*dt;
    mat_set_diag(&temp, diag_val, diag_val, diag_val);
    _mat_copy((double*) temp.m, (double*) F, 3, 3, 6, 6, 0, 3);
//    //F21
    mat_set_diag(&temp, 0.0, 0.0, 0.0);
    _mat_copy((double*) temp.m, (double*) F, 3,3,6,6, 3, 0);
//    //F22
    mat_set_diag(&temp, 1.0, 1.0, 1.0);
    _mat_copy((double*) temp.m, (double*) F,3,3,6,6, 3, 3);

    // update Q
    for(int i=0; i<2; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (i == 0 && j == 0) {
                double val = pow(std_rn_w, 2) * pow(dt, 2);
                mat_set_diag(&temp, val, val, val);
                _mat_copy((double*) temp.m, (double*) Q, 3,3,6,6, 0, 0);
            } else if (i == 1 && j == 1) {
                double val = pow(std_rw_w, 2) * dt;
                mat_set_diag(&temp, val, val, val);
                _mat_copy((double*) temp.m, (double*) Q, 3,3,6,6, 3, 3);
            } else {
                mat_set_diag(&temp, 0.0, 0.0, 0.0);
                _mat_copy((double*) temp.m, (double*) Q, 3,3,6,6, i*3, j*3);
            }
        }
    }

    // update P
    double  Ft[6][6];
    _mat_transpose((double*) F, (double*) Ft, 6,6 );
    double FP[6][6];
    _mat_mat_mult((double*) F, (double*) P, (double*) FP,6,6,6);
    double FPFt[6][6];
    _mat_mat_mult((double*) FP, (double*) Ft, (double*) FPFt, 6,6,6);
    _mat_mat_sum((double*) FPFt, (double*) Q, (double*) P, 6, 6);
}

void eskf_update_mag(vector3_t mag_sensor, vector3_t mag_i, double P[6][6], matrix3_t *R, quaternion_t * q, vector3_t * wb)
{
    // Magnetic Jacobian
    vec_normalize(&mag_i, NULL);
    matrix3_t rwb;
    quat_to_dcm(q, &rwb);
    vector3_t mag_b;
    mat_vec_mult(rwb, mag_i, &mag_b);
    double H[3][6];
    matrix3_t temp;

    mat_skew(mag_b, &temp);
    _mat_copy((double*) temp.m, (double*) H, 3, 3, 3, 6,0,0);
    mat_set_diag(&temp, 0.0, 0.0, 0.0);
    _mat_copy((double*) temp.m, (double*) H, 3, 3, 3, 6,0,3);

    // Kalman Gain
    double Ht[6][3];
    _mat_transpose((double*) H, (double*) Ht, 3, 6);
    double PHt[6][3];
    _mat_mat_mult((double*) P, (double*) Ht, (double*) PHt, 6, 6, 3);
    matrix3_t S1, S, SI;
    _mat_mat_mult((double *)H, (double *) PHt, (double *) S1.m, 3, 6, 3);
    double rval = pow(std_rn_mag,2);
    mat_set_diag(R, rval, rval, rval);
    mat_mat_sum(S1, *R, &S);
    mat_inverse(S, &SI);
    double K[6][3];
    _mat_mat_mult((double*) PHt, (double*) SI.m, (double*) K, 6, 3, 3);

    // Error state update
    vector3_t y, nmag_b;
    vec_cons_mult(-1.0, &mag_b, &nmag_b);
    vec_sum(mag_sensor, nmag_b, &y);
    double dx[6];
    _mat_mat_mult((double*) K, (double*) y.v, (double*)dx, 6 ,3 ,1);


    // Error covariance update
    double KH[6][6], I6[6][6], IKH[6][6], P1[6][6];
    _mat_set_diag((double*) I6, 1.0, 6,6 );
    _mat_mat_mult((double*) K, (double*) H, (double*) KH, 6 ,3, 6);
    _mat_cons_mult(-1.0, (double *) KH, NULL,6, 6);
    _mat_mat_sum((double*) I6, (double*) KH, (double*) IKH, 6, 6);
    _mat_mat_mult((double*) IKH, (double*) P, (double*) P1, 6 ,6, 6);

    // Auxiliar error state variables
    vector3_t dtheta, dwb;
    quaternion_t dq;
    dtheta.v[0] = dx[0]; dtheta.v[1] = dx[1]; dtheta.v[2] = dx[2];
    dwb.v[0] = dx[3]; dwb.v[1] = dx[4]; dwb.v[2] = dx[5];
    vec_to_quat(dtheta, &dq);


    // Injection of the observed error to the nominal state
    quaternion_t q1;
    vector3_t wb1;
    quat_mult(q, &dq, &q1);
    vec_sum(*wb, dwb, &wb1);
    for(int i=0; i < 4; ++i) {
        q->q[i] = q1.q[i];
        if (i > 2) continue;
        wb->v[i] = wb1.v[i];
    }
}

quaternion_t quat_from_dcm(matrix3_t dcm){

    quaternion_t q;

    q.q0 = sqrt(1 + dcm.m[0][0] - dcm.m[1][1] - dcm.m[2][2]) / 2;
    q.q1 = sqrt(1 - dcm.m[0][0] + dcm.m[1][1] - dcm.m[2][2]) / 2;
    q.q2 = sqrt(1 - dcm.m[0][0] - dcm.m[1][1] + dcm.m[2][2]) / 2;
    q.q3 = sqrt(1 + dcm.m[0][0] + dcm.m[1][1] + dcm.m[2][2]) / 2;

    double maxval = 0;
    int maxidx = 0;
    for (int i = 0; i < 4; i++)
    {
        // maximum value
        if (abs_double(q.q[i]) > maxval)
        {
            maxval = abs_double(q.q[i]);
            maxidx = i;
        }
    }
    // Normalize by the maximum value
    switch (maxidx)
    {
        case 0:
            q.q[1] = (dcm.m[0][1] + dcm.m[1][0]) / (4 * q.q[0]);
            q.q[2] = (dcm.m[0][2] + dcm.m[2][0]) / (4 * q.q[0]);
            q.q[3] = (dcm.m[1][2] - dcm.m[2][1]) / (4 * q.q[0]);
            break;
        case 1:
            q.q[0] = (dcm.m[0][1] + dcm.m[1][0]) / (4 * q.q[1]);
            q.q[2] = (dcm.m[2][1] + dcm.m[1][2]) / (4 * q.q[1]);
            q.q[3] = (dcm.m[2][0] - dcm.m[0][2]) / (4 * q.q[1]);
            break;
        case 2:
            q.q[0] = (dcm.m[2][0] + dcm.m[0][2]) / (4 * q.q[2]);
            q.q[1] = (dcm.m[2][1] + dcm.m[1][2]) / (4 * q.q[2]);
            q.q[3] = (dcm.m[0][1] - dcm.m[1][0]) / (4 * q.q[2]);
            break;
        case 3:
            q.q[0] = (dcm.m[1][2] - dcm.m[2][1]) / (4 * q.q[3]);
            q.q[1] = (dcm.m[2][0] - dcm.m[0][2]) / (4 * q.q[3]);
            q.q[2] = (dcm.m[0][1] - dcm.m[1][0]) / (4 * q.q[3]);
            break;
    }
    return q;
}

double abs_double(double x){
    return sqrt(pow(x, 2));
}