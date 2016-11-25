#include "../include/osSemphr.h"

void osSemaphoreCreate(osSemaphore* mutex){
	*mutex = xSemaphoreCreateMutex();
}

void osSemaphoreTake(osSemaphore *mutex, uint32_t timeout){
	xSemaphoreTake(*mutex,timeout);
}

void osSemaphoreGiven(osSemaphore *mutex){
	xSemaphoreGive(*mutex);	
}
