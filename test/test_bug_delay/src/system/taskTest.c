#include "include/taskTest.h"

#define TEST_MAX_SECONDS 5

const static char *tag = "taskTest";

void taskTest(void *param)
{
    char *tag = (char *)param;
    LOGI(tag, "Started");

    int i, work = 0;

    srand(time(NULL));
    portTick xStartTime = osTaskGetTickCount();
    portTick xLastTime = xStartTime;

    for(i=0; i<TEST_MAX_SECONDS; i++)
    {
        portTick xLoopIni = osTaskGetTickCount();
/**
 * Simple failure test: inject one failure so task takes more than 1000ms to
 * execute
 */
#if 1
        // Inject only one failure
        work = 0;
        if(i == 2)
        {
            work = 1500;
            usleep(work * 1000); // Task takes 1500ms in execute
        }
#endif

/**
 * Random test: Simulate random loads in the task between 0-1300ms
 * The task trys to recover from longer loops
 */
#if 0
        work = rand()%1300;
        usleep(work*1000);
#endif
        portTick xLoopEnd = osTaskGetTickCount();
        osTaskDelayUntil(&xLastTime, 1000); //Suspend task for 1 sec
        LOGD(tag, "Iter %02d, tick %u, loop %07d us, sleep %07d us, work %07u us", i, xLastTime, xLastTime-xLoopIni, xLastTime-xLoopEnd, work*1000);
    }

    portTick xFinalTime = osTaskGetTickCount();
    LOGI(tag, "Test executed in %u us, expected ~%f us",
         xFinalTime - xStartTime, TEST_MAX_SECONDS*1000000.0)
    assert(xFinalTime - xStartTime >= 0.999*TEST_MAX_SECONDS*1000000.0);
    exit(0);
}
