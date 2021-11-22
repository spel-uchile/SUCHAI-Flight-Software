/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2021, Camilo Rojas Milla, camrojas@uchile.cl
 *      Copyright 2021, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2021, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2021, Tamara Gutierrez Rojo tamigr.2293@gmail.com
 *      Copyright 2021, Ignacio Ibanez Aliaga, ignacio.ibanez@usach.cl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include "CUnit/Basic.h"
#include "suchai/math_utils.h"
#include "suchai/repoCommand.h"
#include "suchai/storage.h"
#include "app/system/repoDataSchema.h"


/** SUIT 2: Command repository **/
/* The suite initialization function.
 * Initializes
 */
int init_suite_repocmd(void)
{
    int result = cmd_repo_init();
    return result == CMD_OK ? 0 : -1;
}

/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite_repocmd(void)
{
    cmd_repo_close();
    return 0;
}

// Test of parse cmd from string
void testParseCommands(void)
{
    cmd_t *cmd;
    char *name;

    // Case 1: command without parameters; command do not req. parameters.
    cmd = cmd_build_from_str("obc_get_mem");
    CU_ASSERT_PTR_NOT_NULL(cmd);
    name = cmd_get_name(cmd->id);
    CU_ASSERT_STRING_EQUAL("obc_get_mem", name)
    CU_ASSERT_PTR_NULL(cmd->params);
    free(cmd); free(name);

    // Case 2: command with parameters; command do not req. parameters.
    cmd = cmd_build_from_str("obc_get_mem foo");
    CU_ASSERT_PTR_NOT_NULL(cmd);
    name = cmd_get_name(cmd->id);
    CU_ASSERT_STRING_EQUAL("obc_get_mem", name)
    CU_ASSERT_STRING_EQUAL("foo", cmd->params);
    free(cmd); free(name);

    // Case 3: command with parameters; command require parameters.
    cmd = cmd_build_from_str("obc_debug 1");
    CU_ASSERT_PTR_NOT_NULL(cmd);
    name = cmd_get_name(cmd->id);
    CU_ASSERT_STRING_EQUAL("obc_debug", name)
    CU_ASSERT_STRING_EQUAL("1", cmd->params);
    free(cmd); free(name);

    // Case 4: command without parameters; command require parameters.
    cmd = cmd_build_from_str("obc_debug");
    CU_ASSERT_PTR_NOT_NULL(cmd);
    name = cmd_get_name(cmd->id);
    CU_ASSERT_STRING_EQUAL("obc_debug", name)
    CU_ASSERT_PTR_NULL(cmd->params);
    free(cmd); free(name);

    // Case 5: not valid command
    cmd = cmd_build_from_str("invalid_command");
    CU_ASSERT_PTR_NULL(cmd);
    free(cmd);

    // Case 6: empty command
    cmd = cmd_build_from_str("\0");
    CU_ASSERT_PTR_NULL(cmd);
    free(cmd);

    // Case 7: \n or \cr command
    cmd = cmd_build_from_str("\r\n");
    CU_ASSERT_PTR_NULL(cmd);
    free(cmd);
}

/** SUIT 1: Flight Plan **/
/* The suite initialization function.
 * Resets the flight plan
 * Returns zero on success, non-zero otherwise.
 */
int init_suite_fp(void)
{
    int result;
    dat_repo_init();
    result = dat_reset_fp();
    return result;
}

/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite_fp(void)
{
    int result;
    result = dat_reset_fp();
    dat_repo_close();
    return result;
}

// Test of fp_set.
void test_fp_set(void)
{
    int i, result, exec, period;
    char cmd[SCH_CMD_MAX_STR_NAME];
    char args[SCH_CMD_MAX_STR_PARAMS];
    memset(cmd, 0, SCH_CMD_MAX_STR_NAME);
    memset(args, 0, SCH_CMD_MAX_STR_PARAMS);

    // Set flight plan values
    for(i=0; i<10; i++)
    {
        result = dat_set_fp(i, "test_cmd", "arg1 arg2 arg3", i, 0);
        CU_ASSERT_EQUAL(result, 0);
    }

    //dat_show_fp();

    // Read and test flight plan values
    for(i=0; i<10; i++)
    {
        result = dat_get_fp(i, cmd, args, &exec, &period);
        CU_ASSERT_EQUAL(result, 0)
        CU_ASSERT_STRING_EQUAL(cmd, "test_cmd");
        CU_ASSERT_STRING_EQUAL(args, "arg1 arg2 arg3");
        //printf("(%d, %d), (%d, %d)\n", exec,i,period,0);
        CU_ASSERT_EQUAL(exec, i);
        CU_ASSERT_EQUAL(period, 0);
    }
}

//Test of fp_delete
void test_fp_delete(void)
{
    int i, result, exec, period;
    char cmd[SCH_CMD_MAX_STR_NAME];
    char args[SCH_CMD_MAX_STR_PARAMS];
    memset(cmd, 0, SCH_CMD_MAX_STR_NAME);
    memset(args, 0, SCH_CMD_MAX_STR_PARAMS);

    // Set flight plan values
    for(i=0; i<10; i++)
    {
        result = dat_set_fp(i, "test_cmd", "arg1 arg2 arg3", i, 0);
        CU_ASSERT_EQUAL(result, 0);
    }

    // Delete entries and check a deleted entry cannot be read
    for(i=0; i<10; i++)
    {
        result = dat_del_fp(i);
        CU_ASSERT_EQUAL(result, 0);
        result = dat_get_fp(i, cmd, args, &exec, &period);
        CU_ASSERT_EQUAL(result, -1);
    }
}



/** SUIT 3: Data repository **/
/* The suite initialization function.
 * Initializes
 */
int init_suite_repodata(void)
{
    dat_repo_init();
    cmd_repo_init();
    int rc = dat_delete_memory_sections();
    rc += dat_reset_status_vars();
    srand(time(NULL));
    return rc;
}

/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite_repodata(void)
{
    dat_repo_close();
    return 0;
}

//Test of drp_test_system_vars
void test_system_vars(void)
{
    int var_index;
    int var;
    int init_value;
    int test_value = 85;

    for (var_index = 0; var_index < dat_status_last_address; var_index++)
    {
        init_value = dat_get_system_var((dat_status_address_t) var_index);
        dat_set_system_var((dat_status_address_t) var_index, test_value);
        var = dat_get_system_var((dat_status_address_t) var_index);
        dat_set_system_var((dat_status_address_t) var_index, init_value);
        CU_ASSERT_EQUAL(var, test_value)
    }
}

//Test of dat_set_system_var
void test_set_system_vars_fault_tolerant(void)
{
    int rand_ind = rand() % dat_status_last_address;
    int rand_val = rand();
    int val_1, val_2, val_3;

    for (int i = dat_obc_opmode; i < dat_status_last_address; i++)
    {
        if (i == rand_ind)
        {
            dat_set_system_var(i, rand_val);
        }
        else
        {
            dat_set_system_var(i, i + 5);
        }
    }

    for (int i = dat_obc_opmode; i < dat_status_last_address; i++)
    {
        val_1 = _dat_get_system_var(i);
        val_2 = _dat_get_system_var(i + dat_status_last_address);
        val_3 = _dat_get_system_var(i + dat_status_last_address * 2);
        if (i == rand_ind)
        {
            CU_ASSERT_EQUAL(val_1, rand_val);
            CU_ASSERT_EQUAL(val_2, rand_val);
            CU_ASSERT_EQUAL(val_3, rand_val);
        }
        else
        {
            CU_ASSERT_EQUAL(val_1, i + 5);
            CU_ASSERT_EQUAL(val_2, i + 5);
            CU_ASSERT_EQUAL(val_3, i + 5)
        }
    }
}

//Test of dat_get_system_var
void test_get_system_vars_fault_tolerant(void)
{
    int rand_ind = rand() % dat_status_last_address;
    int rand_val = rand();
    int val;

    for (int i = dat_obc_opmode; i < dat_status_last_address; i++)
    {
        if (i == rand_ind)
        {
            _dat_set_system_var(i, 1);
            _dat_set_system_var(i + dat_status_last_address, 2);
            _dat_set_system_var(i + 2 * dat_status_last_address, 3);
        }
        else
        {
            _dat_set_system_var(i, rand_val);
            _dat_set_system_var(i + dat_status_last_address, rand_val);
            _dat_set_system_var(i + 2 * dat_status_last_address, rand_val);
        }
    }

    for (int i = dat_obc_opmode; i < dat_status_last_address; i++)
    {
        val = dat_get_system_var(i);

        if (i == rand_ind)
        {
            CU_ASSERT_EQUAL(val, 1);
        }

        else
        {
            CU_ASSERT_EQUAL(val, rand_val);
        }
    }
}

void test_payload_data_1(void)
{
    init_suite_repodata();

    int rc, i;
    int n_test = 10;
    uint32_t time_test = (uint32_t)time(NULL);
    int32_t int_test = rand();
    float float_test = rand()/(float)rand();

    // Push N data samples to the storage system
    for(i=0; i<n_test; i++)
    {
        float_data_t data;
        data.timestamp = time_test + i;
        data.index = (uint32_t)i;
        data.data1 = int_test - i;
        data.data2 = float_test + i;
        rc = dat_add_payload_sample(&data, float_data);
        CU_ASSERT(rc >= 0);
    }

    // Test storage payload index
    CU_ASSERT_EQUAL(dat_get_system_var(data_map[float_data].sys_index), n_test);

    // Read N data samples to the storage system
    for(i=0; i<n_test; i++)
    {
        float_data_t data;
        rc = dat_get_recent_payload_sample(&data, float_data, n_test-i-1);
        //dat_print_payload_struct(&data, float_data);
        CU_ASSERT_EQUAL(rc, 0);
        CU_ASSERT_EQUAL(data.timestamp, time_test+i);
        CU_ASSERT_EQUAL(data.data1, int_test-i);
        CU_ASSERT_DOUBLE_EQUAL(data.data2, float_test+i, 1e-6);
    }
}

void test_payload_data_2(void)
{
    init_suite_repodata();

    int rc, i;
    int n_test = 10;
    uint32_t time_test = (uint32_t)time(NULL);
    int16_t data_test = (int16_t)(rand()/2);

    // Push N data samples to the storage system
    for(i=0; i<n_test; i++)
    {
        half_data_t data;
        data.timestamp = time_test + i;
        data.index = (uint32_t)i;
        data.data1 = data_test - i;
        data.data2 = data_test + i;
        rc = dat_add_payload_sample(&data, half_data);
        CU_ASSERT(rc >= 0);
    }

    // Test storage payload index
    CU_ASSERT_EQUAL(dat_get_system_var(data_map[half_data].sys_index), n_test);

    // Read N data samples to the storage system
    for(i=0; i<n_test; i++)
    {
        half_data_t data;
        rc = dat_get_recent_payload_sample(&data, half_data, n_test-i-1);
        //dat_print_payload_struct(&data, half_data);
        CU_ASSERT_EQUAL(rc, 0);
        CU_ASSERT_EQUAL(data.timestamp, time_test+i);
        CU_ASSERT_EQUAL(data.data1, data_test-i);
        CU_ASSERT_EQUAL(data.data2, data_test+i);
    }
}

void test_payload_data_3(void)
{
    init_suite_repodata();

    int rc, i;
    int n_test = 10;
    uint32_t time_test = (uint32_t)time(NULL);
    char *str1 = "test short string";
    char *str2 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi scelerisque sapien velit, in tristique orci dictum vel. In duis.";

    // Push N data samples to the storage system
    for(i=0; i<n_test; i++)
    {
        string_data_t data;
        data.timestamp = time_test + i;
        data.index = (uint32_t)i;
        memset(data.str1, 0, SCH_ST_STR_SIZE);
        memset(data.str2, 0, SCH_ST_STR_SIZE);
        strcpy(data.str1, str1);
        strcpy(data.str2, str2);
        rc = dat_add_payload_sample(&data, string_data);
        CU_ASSERT(rc >= 0);
    }

    // Test storage payload index
    CU_ASSERT_EQUAL(dat_get_system_var(data_map[string_data].sys_index), n_test);

    // Read N data samples to the storage system
    for(i=0; i<n_test; i++)
    {
        string_data_t data;
        rc = dat_get_recent_payload_sample(&data, string_data, n_test-i-1);
        //dat_print_payload_struct(&data, string_data);
        CU_ASSERT_EQUAL(rc, 0);
        CU_ASSERT_EQUAL(data.timestamp, time_test+i);
        CU_ASSERT_STRING_EQUAL(data.str1, str1);
        CU_ASSERT_STRING_EQUAL(data.str2, str2);
    }
}

void test_payload_data_4(void)
{
    init_suite_repodata();

    int rc, i;
    int n_test = 10;
    uint32_t time_test = (uint32_t)time(NULL);
    int32_t int_test = rand();
    int16_t short_test = (int16_t)(rand()/2);
    char *str1 = "test short string";

    // Push N data samples to the storage system
    for(i=0; i<n_test; i++)
    {
        mix_data_t data;
        data.timestamp = time_test + i;
        data.index = (uint32_t)i;
        data.data1 = short_test + i;
        data.data2 = short_test - i;
        data.data3 = int_test + i;
        memset(data.str1, 0, SCH_ST_STR_SIZE);
        strcpy(data.str1, str1);
        rc = dat_add_payload_sample(&data, mix_data);
        CU_ASSERT(rc >= 0);
    }

    // Read N data samples to the storage system
    for(i=0; i<n_test; i++)
    {
        mix_data_t data;
        rc = dat_get_recent_payload_sample(&data, mix_data, n_test-i-1);
        //dat_print_payload_struct(&data, mix_data);
        CU_ASSERT_EQUAL(rc, 0);
        CU_ASSERT_EQUAL(data.timestamp, time_test+i);
        CU_ASSERT_EQUAL(data.data1, short_test + i);
        CU_ASSERT_EQUAL(data.data2, short_test - i);
        CU_ASSERT_EQUAL(data.data3, int_test + i);
        CU_ASSERT_STRING_EQUAL(data.str1, str1);
    }
}

void test_payload_delete(void)
{
    /*
     * Fill payloads A and B with data, delete payload A one, check A status variables are consistent
     * Check status variables and data for payload B were not affected
     * Re-fill delete payload and check A status variables are consistent
     */

    init_suite_repodata();

    int rc, i;
    int n_test = 10;
    uint32_t time_test = (uint32_t)time(NULL);
    int32_t int_test = rand();
    float float_test = rand()/(float)rand();
    int16_t data_test = (int16_t)(rand()/2);


    // Push N data samples to payload A storage system
    for(i=0; i<n_test; i++)
    {
        float_data_t data;
        data.timestamp = time_test + i;
        data.index = (uint32_t)i;
        data.data1 = int_test - i;
        data.data2 = float_test + i;
        rc = dat_add_payload_sample(&data, float_data);
        CU_ASSERT(rc >= 0);
    }

    // Push N data samples to payload B the storage system
    for(i=0; i<n_test; i++)
    {
        half_data_t data;
        data.timestamp = time_test + i;
        data.index = (uint32_t)i;
        data.data1 = data_test - i;
        data.data2 = data_test + i;
        rc = dat_add_payload_sample(&data, half_data);
        CU_ASSERT(rc >= 0);
    }

    // Test status variables for A and B
    CU_ASSERT_EQUAL(dat_get_system_var(data_map[float_data].sys_index), n_test);
    CU_ASSERT_EQUAL(dat_get_system_var(data_map[half_data].sys_index), n_test);

    // Delete payload A
    rc = dat_delete_payload(float_data);
    CU_ASSERT_EQUAL(rc, SCH_ST_OK);

    // Test status variables for A and B
    CU_ASSERT_EQUAL(dat_get_system_var(data_map[float_data].sys_index), 0);
    CU_ASSERT_EQUAL(dat_get_system_var(data_map[half_data].sys_index), n_test);

    // Push N new data samples to payload A storage system
    time_test += 1;
    int_test = rand();
    float_test = rand()/(float)rand();
    for(i=0; i<n_test; i++)
    {
        float_data_t data;
        data.timestamp = time_test + i;
        data.index = (uint32_t)i;
        data.data1 = int_test - i;
        data.data2 = float_test + i;
        rc = dat_add_payload_sample(&data, float_data);
        CU_ASSERT(rc >= 0);
    }

    // Read N data samples to the storage system and check these are the new values, not the deleted one
    for(i=0; i<n_test; i++)
    {
        float_data_t data;
        rc = dat_get_payload_sample(&data, float_data, i);
        CU_ASSERT_EQUAL(rc, 0);
        CU_ASSERT_EQUAL(data.timestamp, time_test+i);
        CU_ASSERT_EQUAL(data.data1, int_test-i);
        CU_ASSERT_DOUBLE_EQUAL(data.data2, float_test+i, 1e-6);
    }
}

/** SUIT 4 **/
/* The suite initialization function.
 * Initializes
 */
int init_suite_quat(void)
{
    return 0;
}

/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite_quat(void)
{
    return 0;
}

//Test of quaternion library
void testQUATERNION(void)
{
    quaternion_t q;
    quaternion_t q1;
    quaternion_t q2;
    quaternion_t q3;
    vector3_t vec;
    vector3_t new_vec;

    vec.v[0] = 2; vec.v[1] = -5; vec.v[2] = -1;
    q1.q[0] = -1; q1.q[1] = -0.5; q1.q[2] = 4; q1.q[3] = 2;

    /**
     * Testing quaternion identity
     * I = Q x Q*
     */
    quat_normalize(&q1, &q2);
    quat_inverse(&q1, &q3);
    quat_mult(&q3, &q2, &q);

    CU_ASSERT_DOUBLE_EQUAL(q.q0, 0.0, 1e-6);
    CU_ASSERT_DOUBLE_EQUAL(q.q1, 0.0, 1e-6);
    CU_ASSERT_DOUBLE_EQUAL(q.q2, 0.0, 1e-6);
    CU_ASSERT_DOUBLE_EQUAL(q.q3, 1.0, 1e-6);

    /**
     * Testing known convertion
     */
    quat_frame_conv(&q2, &vec, &new_vec);
    CU_ASSERT_DOUBLE_EQUAL(new_vec.v0, -4.7764705882352940, 1e-6);
    CU_ASSERT_DOUBLE_EQUAL(new_vec.v1,  1.9647058823529413, 1e-6);
    CU_ASSERT_DOUBLE_EQUAL(new_vec.v2, -1.8235294117647058, 1e-6);
}


/** SUIT 5 **/
/* Test matrices.
 */

int init_suite_mat(void)
{
    return 0;
}

/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite_mat(void)
{
    return 0;
}

void check_values(double* list, double* ref, int length)
{
    for(int i =0; i < length; ++i)
    {
        CU_ASSERT_DOUBLE_EQUAL(list[i], ref[i], 1e-6);
    }
}

void testMatrices(void)
{
    /*
    * Testing 3x3 matrices
    */
    matrix3_t A = {0.34197022, 0.4621773 , 0.61930235, 0.53479301, 0.19789368,
    0.82224241, 0.26827125, 0.30458023, 0.33275187};
    matrix3_t B = {0.53577991, 0.07315265, 0.95531149, 0.69947871, 0.49924559,
                   0.97280217, 0.44754281, 0.71025288, 0.74547963};
    vector3_t v = {0.8134576 , 0.05684925, 0.83800431};
    matrix3_t I;
    mat_set_diag(&I, 1.0, 1.0, 1.0);
    matrix3_t res;
    vector3_t resv;

    // Refs
    matrix3_t sumAB = {0.87775013, 0.53532995, 1.57461384, 1.23427171, 0.69713927,
                       1.79504459, 0.71581406, 1.01483311, 1.07823151};
    matrix3_t mulAB = {0.78366827, 0.69561729, 1.23797245, 0.79294244, 0.72191912,
                       1.31637028, 0.50570244, 0.40802307, 0.80063866};
    matrix3_t At = {0.34197022, 0.53479301, 0.26827125, 0.4621773 , 0.19789368,
                    0.30458023, 0.61930235, 0.82224241, 0.33275187};
    matrix3_t Ainv = {-7.51060729,  1.41745058, 10.47582009,  1.73456389, -2.13001898,
                      2.03506727,  4.46749152,  0.80690881, -7.30334774};
    vector3_t Av = {0.82343075, 1.13532423, 0.51438995};

    // Test identity
    mat_mat_mult(A, I, &res);
    check_values((double*)res.m, (double*)A.m, 9);
    // Test sum
    mat_mat_sum(A, B, &res);
    check_values((double*)res.m, (double*)sumAB.m, 9);
    // Test mul
    mat_mat_mult(A, B, &res);
    check_values((double*)res.m, (double*)mulAB.m, 9);
    // Test transpose
    mat_transpose(&A, &res);
    check_values((double*)res.m, (double*)At.m, 9);
    // Test inverse
    mat_inverse(A, &res);
    check_values((double*)res.m, (double*)Ainv.m, 9);
    matrix3_t resaux;
    mat_mat_mult(A, res, &resaux);
    check_values((double*)resaux.m, (double*)I.m, 9);
    // Test matrix vector multiplication
    mat_vec_mult(A, v, &resv);
    check_values((double*)resv.v, (double*)Av.v, 3);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /**
     * SUITE 1: Flight Plan unit tests
     */
    pSuite = CU_add_suite("Suite Flight Plan", init_suite_fp, clean_suite_fp);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test of fpset()", test_fp_set))
           || (NULL == CU_add_test(pSuite, "test of fpdelete()", test_fp_delete))){
        CU_cleanup_registry();
        return CU_get_error();
    }

    /**
     * SUITE 2: Repo command unit tests
     */
    pSuite = CU_add_suite("Suite repo command", init_suite_repocmd, clean_suite_repocmd);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test of cmd_build_from_str()", testParseCommands))){
        CU_cleanup_registry();
        return CU_get_error();
    }

    /**
     * SUITE 3: Repo data unit tests
     */
    pSuite = CU_add_suite("Suite repo data", init_suite_repodata, clean_suite_repodata);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test of drp_test_system_vars", test_system_vars)) ||
            (NULL == CU_add_test(pSuite, "test of dat_set_system_var", test_set_system_vars_fault_tolerant)) ||
            (NULL == CU_add_test(pSuite, "test of dat_get_system_var", test_get_system_vars_fault_tolerant)) ||
            (NULL == CU_add_test(pSuite, "test of payload storage 1", test_payload_data_1)) ||
            (NULL == CU_add_test(pSuite, "test of payload storage 2", test_payload_data_2)) ||
            (NULL == CU_add_test(pSuite, "test of payload storage 3", test_payload_data_3)) ||
            (NULL == CU_add_test(pSuite, "test of payload storage 4", test_payload_data_4)) ||
            (NULL == CU_add_test(pSuite, "test of payload storage delete", test_payload_delete))
        )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /**
     * SUITE 4: Quaternions in math_utils.c
     */
    pSuite = CU_add_suite("Suite quaternions", init_suite_quat, clean_suite_quat);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test of quaternions", testQUATERNION))){
        CU_cleanup_registry();
        return CU_get_error();
    }

    /**
    * SUITE 5: Matrices math_utils.c
    */
    pSuite = CU_add_suite("Suite matrices", init_suite_mat, clean_suite_mat);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test of matrices", testMatrices))){
        CU_cleanup_registry();
        return CU_get_error();
    }


    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    int failed;
    failed = CU_get_number_of_tests_failed();

    CU_cleanup_registry();

    /*if one or more tests have failed, the execution needs to be ended with exit code 1*/
    if (failed != 0) {
        exit(1);
    }

    return CU_get_error();
}
