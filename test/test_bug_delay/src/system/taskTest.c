#include "include/taskTest.h"

#define TEST_FAILS 1
#define TEST_MAX_SECONDS 5

const static char *tag = "taskTest";

void taskTest(void *param)
{
    char *tag = (char *)param;
    LOGI(tag, "Started");

    int i = 0;

    portTick xStartTime = osTaskGetTickCount();
    portTick xLastTime = xStartTime;

    for(i=0; i<TEST_MAX_SECONDS; i++)
    {
        portTick xTmpTime = xLastTime;
        osTaskDelayUntil(&xLastTime, 1000); //Suspend task for 1 sec
        LOGD(tag, "Iter %d, tick %u, dt %u us", i, xLastTime, xLastTime-xTmpTime);
        #if TEST_FAILS
        if(i == 1)
            sleep(2);
        #endif
    }

    portTick xFinalTime = osTaskGetTickCount();
    LOGI(tag, "Test executed in %u us, expected ~%f us",
         xFinalTime - xStartTime, TEST_MAX_SECONDS*1000000.0)
    assert(xFinalTime - xStartTime >= 0.999*TEST_MAX_SECONDS*1000000.0);
    exit(0);
}
