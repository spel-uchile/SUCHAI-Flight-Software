/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 Gomspace ApS (http://www.gomspace.com)
Copyright (C) 2012 AAUSAT3 Project (http://aausat3.space.aau.dk) 

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _OS_QUEUE_H_
#define _OS_QUEUE_H_

#include "../../SUCHAI_config.h"
#include <stdint.h>

#if __linux__
	#include "../include/pthread_queue.h"
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
