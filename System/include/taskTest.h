#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "SUCHAI_config.h"

//#include "csp.h"
//#include "csp_if_i2c.h"
//#include "csp_i2c.h"

void taskTest(void *param);
void taskTestCSP(void *param);

#endif

