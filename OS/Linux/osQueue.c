#include "../include/osQueue.h"

osQueue osQueueCreate(int length, size_t item_size) {
	return pthread_queue_create(length, item_size);
}

int osQueueSend(osQueue queue, void * value, uint32_t timeout) {
	return pthread_queue_send(queue, value, timeout);
}

int osQueueReceive(osQueue queue, void * buf, uint32_t timeout){
    return pthread_queue_receive(queue, buf, timeout);
}

