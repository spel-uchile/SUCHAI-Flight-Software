#ifndef _OS_SEMPHR_H_
#define _OS_SEMPHR_H_

#include "../../SUCHAI_config.h"

#if __linux__
	#include <pthread.h>
	typedef pthread_mutex_t osSemaphore;

#else
	#include "FreeRTOS.h"
	#include "semphr.h"
	typedef xSemaphoreHandle osSemaphore;
#endif

void osSemaphoreCreate(osSemaphore* mutex);
void osSemaphoreTake(osSemaphore* mutex, uint32_t timeout); 
void osSemaphoreGiven(osSemaphore* mutex);

#endif