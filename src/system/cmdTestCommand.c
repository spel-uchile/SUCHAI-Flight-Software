/**
 * @file  cmdTestCommand.c
 * @author Ignacio Ibañez A - ignacio.ibanez@usach.cl
 * @date 2016
 * @copyright GNU GPL v3
 *
 * This file have tests related to commands interface
 */

#include "cmdTestCommand.h"

static const char *tag = "cmdTest";

void cmd_test_init(void)
{

    cmd_add("test_str_int", test_cmd_str_int, "%s %i", 2);
    cmd_add("test_double_int", test_cmd_double_int, "%f %f %i %i", 4);
    cmd_add("test_str_double_int", test_cmd_str_double_int, "%s %f %s %f %i", 5);

}

int test_cmd_str_int(char *fmt, char *params, int nparams)
{
    char msg[10];
    int valor = 0;

    errno = 0;
    assertf(sscanf(params,fmt, msg, &valor) == nparams, tag, "The format of parameters are: %s and parameters used are: %s",fmt, params);
    assertf(errno == 0, tag, "The format of parameters are: %s and parameters used are: %s",fmt, params);
    LOGI(tag, "%s: %s_%i","con_str_int", msg, valor);
    return CMD_OK;
}

int test_cmd_double_int(char *fmt, char *params, int nparams)
{
    float v1 = 0, v2 = 0;
    int v3 = 0,v4 = 0;

    assertf(sscanf(params,fmt, &v1, &v2, &v3, &v4) == nparams, tag, "The format of parameters are: %s and parameters used are: %s",fmt, params);
    LOGI(tag, "%s: %f_%f_%i_%i", "con_double_int",v1,v2,v3,v4);
    return CMD_OK;
}

int test_cmd_str_double_int(char *fmt, char *params, int nparams)
{
    char v1[10];
    char v3[10];
    float v2 = 0, v4 = 0;
    int v5 = 0;

    assertf( sscanf(params,fmt, v1, &v2, v3, &v4, &v5) == nparams, tag, "The format of parameters are: %s and parameters used are: %s",fmt, params);
    LOGI(tag, "%s: %s_%f_%s_%f_%i","str_double_int",v1,v2,v3,v4,v5);
    return CMD_OK;
}
