#include "taskTest.h"
#include <i2c.h>

void taskTest(void *param)
{
    const unsigned long Delayms = 500 / portTICK_RATE_MS;

    #if SCH_GRL_VERBOSE
        printf("[TaskTest] %s\n", (char*)param);
    #endif
    
    cmd_print_all();
    cmd_t cmd;
    
    while(1)
    {
        vTaskDelay(Delayms);
        cmd = cmd_get_str("test");
        printf("[taskTest] New cmd: %s, %i.\n", cmd.name, cmd.id);
        cmd.function((void *)"1");
        
        vTaskDelay(Delayms);
        cmd = cmd_get_str("none");
        printf("[taskTest] New cmd: %s, %i.\n", cmd.name, cmd.id);
        cmd.function(NULL);
    }
}

//void taskTestCSP(void *param)
//{
//    const unsigned long Delayms = 5000 / portTICK_RATE_MS;
//    int csp_node = *(int *)param;
//
//    /* Init buffer system with 3 packets of maximum 256 bytes each */
//    csp_buffer_init(3, 256);
//
//    /* Init CSP with address MY_ADDRESS */
//    csp_init(csp_node);
//
//    i2c_init(0, I2C_MASTER, 0xAA, 400, 2, 2, NULL);
//    csp_i2c_init(0xAA, 0, 400);
//
//    csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_i2c, CSP_NODE_MAC);
//    csp_route_start_task(100, 2);
//
//    int pingResult;
//
//    while(1)
//    {
//        vTaskDelay(Delayms);
//
//        pingResult = csp_ping(10, 3000, 100, CSP_O_NONE);
//        #if SCH_GRL_VERBOSE
//            printf("Ping with payload of %d bytes, took %d ms\n", 100, pingResult);
//        #endif
//    }
//}
