/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Camilo Rojas Milla, camrojas@uchile.cl
 *      Copyright 2020, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2020, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2020, Tamara Gutierrez Rojo tamigr.2293@gmail.com
 *      Copyright 2020, Ignacio Ibanez Aliaga, ignacio.ibanez@usach.cl
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

#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "cmdFP.h"
#include "cmdOBC.h"
#include "repoCommand.h"

/* The suite initialization function.
 * Resets the flight plan
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void)
{
    dat_repo_init();
    fp_reset("", "", 1);
    return 0;
}

/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void)
{
    return 0;
}

/* The suite initialization function.
 * Initializes
 */
int init_suite2(void)
{
    cmd_repo_init();
    return 0;
}

/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite2(void)
{
    return 0;
}

/* The suite initialization function.
 * Initializes
 */
int init_suite3(void)
{
    dat_repo_init();
    cmd_repo_init();
    drp_execute_before_flight("%d", "1010", 1);
    srand(time(NULL));
    return 0;
}

/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite3(void)
{
    return 0;
}

// Test of parse cmd from string
void testParseCommands(void)
{
    cmd_t *cmd;
    char *name;

    // Case 1: command without parameters; command do not req. parameters.
    cmd = cmd_parse_from_str("obc_get_mem");
    CU_ASSERT_PTR_NOT_NULL(cmd);
    name = cmd_get_name(cmd->id);
    CU_ASSERT_STRING_EQUAL("obc_get_mem", name)
    CU_ASSERT_PTR_NULL(cmd->params);
    free(cmd); free(name);

    // Case 2: command with parameters; command do not req. parameters.
    cmd = cmd_parse_from_str("obc_get_mem foo");
    CU_ASSERT_PTR_NOT_NULL(cmd);
    name = cmd_get_name(cmd->id);
    CU_ASSERT_STRING_EQUAL("obc_get_mem", name)
    CU_ASSERT_STRING_EQUAL("foo", cmd->params);
    free(cmd); free(name);

    // Case 3: command with parameters; command require parameters.
    cmd = cmd_parse_from_str("obc_debug 1");
    CU_ASSERT_PTR_NOT_NULL(cmd);
    name = cmd_get_name(cmd->id);
    CU_ASSERT_STRING_EQUAL("obc_debug", name)
    CU_ASSERT_STRING_EQUAL("1", cmd->params);
    free(cmd); free(name);

    // Case 4: command without parameters; command require parameters.
    cmd = cmd_parse_from_str("obc_debug");
    CU_ASSERT_PTR_NOT_NULL(cmd);
    name = cmd_get_name(cmd->id);
    CU_ASSERT_STRING_EQUAL("obc_debug", name)
    CU_ASSERT_PTR_NULL(cmd->params);
    free(cmd); free(name);

    // Case 5: not valid command
    cmd = cmd_parse_from_str("invalid_command");
    CU_ASSERT_PTR_NULL(cmd);
    free(cmd);

    // Case 6: empty command
    cmd = cmd_parse_from_str("\0");
    CU_ASSERT_PTR_NULL(cmd);
    free(cmd);

    // Case 7: \n or \cr command
    cmd = cmd_parse_from_str("\r\n");
    CU_ASSERT_PTR_NULL(cmd);
    free(cmd);
}

// Test of fp_set.
void testFPSET(void)
{
    char* fmt = "%d %d %d %d %d %d %d %d %s %n";
    char* params = "26 01 2020 12 35 00 1 0 helloworld 1 2 3 3 4";
    int nparams = 10;
    int result;
    result = fp_set(fmt, params, nparams);
    CU_ASSERT(CMD_OK == result);
}

//Test of fp_delete
void testFPDELETE(void)
{
    char* fmt = "%d %d %d %d %d %d";
    char* params = "26 01 2020 12 35 00";
    int nparams = 6;
    int result;
    result = fp_delete(fmt, params, nparams);
    CU_ASSERT_EQUAL(CMD_OK, result);
}

//Test of drp_test_system_vars
void testSYSVARS(void)
{
    int result;
    result = drp_test_system_vars("", "", 0);
    CU_ASSERT(CMD_OK == result);
}

//Test of dat_set_system_var
void testDATSET_SYSVAR(void)
{
    int rand_ind = rand() % dat_system_last_var;
    int rand_val = rand();
    int val_1, val_2, val_3;

    for (int i = dat_obc_opmode; i < dat_system_last_var; i++)
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

    for (int i = dat_obc_opmode; i < dat_system_last_var; i++)
    {
        val_1 = _dat_get_system_var(i);
        val_2 = _dat_get_system_var(i + dat_system_last_var);
        val_3 = _dat_get_system_var(i + dat_system_last_var * 2);
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
void testDATGET_SYSVAR(void)
{
    int rand_ind = rand() % dat_system_last_var;
    int rand_val = rand();
    int val;

    for (int i = dat_obc_opmode; i < dat_system_last_var; i++)
    {
        if (i == rand_ind)
        {
            _dat_set_system_var(i, 1);
            _dat_set_system_var(i + dat_system_last_var, 2);
            _dat_set_system_var(i + 2 * dat_system_last_var, 3);
        }
        else
        {
            _dat_set_system_var(i, rand_val);
            _dat_set_system_var(i + dat_system_last_var, rand_val);
            _dat_set_system_var(i + 2 * dat_system_last_var, rand_val);
        }
    }

    for (int i = dat_obc_opmode; i < dat_system_last_var; i++)
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

/** SUIT 4 **/
/* The suite initialization function.
 * Initializes
 */
int init_suite4(void)
{
    return 0;
}

/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite4(void)
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

    /*
     * SUITE 1: Flight Plan unit tests
     */
    pSuite = CU_add_suite("Suite Flight Plan", init_suite1, clean_suite1);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test of fpset()", testFPSET))
           || (NULL == CU_add_test(pSuite, "test of fpdelete()", testFPDELETE))){
        CU_cleanup_registry();
        return CU_get_error();
    }

    /*
     * SUITE 2: Repo command unit tests
     */
    pSuite = CU_add_suite("Suite repo command", init_suite2, clean_suite2);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test of cmd_parse_from_str()", testParseCommands))){
        CU_cleanup_registry();
        return CU_get_error();
    }

    /*
     * SUITE 3: Repo data unit tests
     */
    pSuite = CU_add_suite("Suite repo data", init_suite3, clean_suite3);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test of drp_test_system_vars", testSYSVARS)) ||
            (NULL == CU_add_test(pSuite, "test of dat_set_system_var", testDATSET_SYSVAR)) ||
            (NULL == CU_add_test(pSuite, "test of dat_get_system_var", testDATGET_SYSVAR))){
        CU_cleanup_registry();
        return CU_get_error();
    }

    /*
     * SUITE 4: Quaternions in utils.c
     */
    pSuite = CU_add_suite("Suite quaternions", init_suite4, clean_suite4);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test of quaternions", testQUATERNION))){
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


