#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "cmdFP.h"

/* The suite initialization function.
 * Resets the flight plan
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void)
{
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

// Test of fp_set.
void testFPSET(void)
{
    char* fmt = "%d %d %d %d %d %d %s %s %d %d";
    char* params = "26 01 2018 12 35 00 helloworld 1,2,3,3,4 1 0";
    int nparams = 10;
    int result;
    result = fp_set(fmt, params, nparams);
    CU_ASSERT(CMD_OK == result);
}

//Test of fp_delete
void testFPDELETE(void)
{
    char* fmt = "%d %d %d %d %d %d";
    char* params = "26 01 2018 12 35 00";
    int nparams = 6;
    int result;
    result = fp_delete(fmt, params, nparams);
    CU_ASSERT(CMD_OK == result);
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

    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
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


