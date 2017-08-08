/**
 * @file  cmdTestCommand.h
 * @author Ignacio Ibañez A - ignacio.ibanez@usach.cl
 * @date 2016
 * @copyright GNU GPL v3
 *
 * This header have tests related to command interface
 */

#ifndef CMD_TEST_COMMAND_H
#define CMD_TEST_COMMAND_H

#include <errno.h>
#include <assert.h>

#include "SUCHAI_config.h"
#include "utils.h"
#include "repoCommand.h"

/* Function definitions */
/**
 * Initializes command repository the cmdTestCommand.h
 *
 * @param None
 * @return None
 */
void test_cmd_init(void);

/**
 * Test commands with string and integers
 *
 * @param fparams Pointer to parameter format
 * @param params Pointer to parameters
 * @param nparam Numbers of parameters
 * @return  One if the command was executed correctly
 *          Zero if the command failed
 */
int test_cmd_str_int(char *fmt, char *params, int nparams);

/**
 * Test commands with double and integers
 *
 * @param fparams Pointer to parameter format
 * @param params Pointer to parameters
 * @param nparam Numbers of parameters
 * @return  One if the command was executed correctly
 *          Zero if the command failed
 */
int test_cmd_double_int(char *fmt, char *params, int nparams);

/**
 * Test commands with string, double and integers
 *
 * @param fparams Pointer to parameter format
 * @param params Pointer to parameters
 * @param nparam Numbers of parameters
 * @return  One if the command was executed correctly
 *          Zero if the command failed
 */
int test_cmd_str_double_int(char *fmt, char *params, int nparams);


#endif //CMD_TEST_COMMAND_H
