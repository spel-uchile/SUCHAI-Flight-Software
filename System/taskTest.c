#include "include/taskTest.h"

//extern os_queue xQueue1; 

void taskTest(void *param)
{
    const unsigned long Delayms = osDefineTime(500);
    char* msg = (char *)param;
    //int enviado = 0;
    //int recibido = 0;
    
    while(1)
    {   
        osDelay(Delayms);
        
        #if SCH_GRL_VERBOSE
            printf("[Test] running...: %s\r\n", msg);
        #endif
        /*if(strcmp(msg,"Thread 1") == 0){
            if(!os_queue_send(xQueue1, &enviado,3000)){
                printf("Fallido mensaje enviado\n");
            }else{
                enviado = enviado + 2;
            }
        }else{
            if(os_queue_receive(xQueue1,&recibido, 3000)){
                recibido++;
                printf("Received %i\n", recibido);
            }else{
                printf("Error al recibir mensaje\n");
            }
        }*/
    }
}
