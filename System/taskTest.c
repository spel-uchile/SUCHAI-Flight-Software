#include "taskTest.h"
#include "SUCHAI_config.h"

void taskTest(void *param)
{
    const unsigned long Delayms = 500 / portTICK_RATE_MS;

    while(1)
    {
        vTaskDelay(Delayms);

        #if SCH_GRL_VERBOSE
            printf("[Test] running... \r\n");
        #endif
    }
}
