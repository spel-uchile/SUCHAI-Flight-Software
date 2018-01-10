#include "osSemphr.h"

int osSemaphoreCreate(osSemaphore* mutex){
	*mutex = xSemaphoreCreateMutex();
	if (*mutex) {
		return CSP_SEMAPHORE_OK;
	} else {
		return CSP_SEMAPHORE_ERROR;
	}
}

int osSemaphoreTake(osSemaphore *mutex, uint32_t timeout){
	if (timeout != portMAX_DELAY)
		timeout = timeout / portTICK_RATE_MS;
	if (xSemaphoreTake(*mutex, timeout) == pdPASS) {
		return CSP_SEMAPHORE_OK;
	} else {
		return CSP_SEMAPHORE_ERROR;
	}
}

int osSemaphoreGiven(osSemaphore *mutex){
	if (xSemaphoreGive(*mutex) == pdPASS) {
		return CSP_SEMAPHORE_OK;
	} else {
		return CSP_SEMAPHORE_ERROR;
	}
}
