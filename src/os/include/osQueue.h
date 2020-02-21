/**
 * @file  osQueue.h
 * @author Carlos Gonzalez Cortes
 * @author Ignacio Ibanez Aliaga
 * @date 2020
 * @copyright GNU Public License.
 */

#ifndef _OS_QUEUE_H_
#define _OS_QUEUE_H_

#include <stdint.h>
#include "config.h"

#ifdef LINUX
	#include "pthread_queue.h"
#else
    #include "FreeRTOS.h"
    #include "queue.h"
#endif

typedef void* osQueue;

osQueue osQueueCreate(int length, size_t item_size);
int osQueueSend(osQueue queues, void *value, uint32_t timeout);
int osQueueReceive(osQueue queue, void *buf, uint32_t timeout);
//void os_queue_remove(csp_queue_handle_t queue);
//int os_queue_enqueue(csp_queue_handle_t handle, void *value, uint32_t timeout);
//int os_queue_enqueue_isr(csp_queue_handle_t handle, void * value, CSP_BASE_TYPE * task_woken);
//int os_queue_dequeue(csp_queue_handle_t handle, void *buf, uint32_t timeout);
//int os_queue_dequeue_isr(csp_queue_handle_t handle, void * buf, CSP_BASE_TYPE * task_woken);
//int os_queue_size(csp_queue_handle_t handle);
//int os_queue_size_isr(csp_queue_handle_t handle);

#endif
