/**
 * @file  cmdTestCommand.h
 * @author Ignacio Ibanez A - ignacio.ibanez@usach.cl
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2017
 * @copyright GNU GPL v3
 *
 * This header have tests related to command interface
 */

#ifndef CMD_TEST_COMMAND_H
#define CMD_TEST_COMMAND_H

#include <errno.h>
#include <assert.h>

#include "suchai/config.h"
#include "suchai/log_utils.h"
#include "repoCommand.h"

/* Function definitions */
/**
 * Initializes command repository the cmdTestCommand.h
 *
 * @param None
 * @return None
 */
void cmd_test_init(void);

/**
 * Test commands with string and integers
 *
 * @param fparams Str. Parameters format "%s %i"
 * @param params Str. Parameters as string "abc 123"
 * @param nparam Int. Number of parameters 2
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int test_cmd_str_int(char *fmt, char *params, int nparams);

/**
 * Test commands with double and integers
 *
 * @param fparams Str. Parameters format "%f %f %i %i"
 * @param params Str. Parameters as string "1.2 3.4 5 6"
 * @param nparam Int. Number of parameters 4
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int test_cmd_double_int(char *fmt, char *params, int nparams);

/**
 * Test commands with string, double and integers
 *
 * @param fparams Str. Parameters format "%s %f %s %f %i"
 * @param params Str. Parameters as string "abc 12.3 cde 4.56 789"
 * @param nparam Int. Number of parameters 5
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int test_cmd_str_double_int(char *fmt, char *params, int nparams);


#endif //CMD_TEST_COMMAND_H
