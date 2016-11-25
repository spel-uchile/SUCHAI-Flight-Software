
#include "../include/osQueue.h"

osQueue osQueueCreate(int length, size_t item_size) {
	return xQueueCreate(length, item_size);
}

int osQueueSend(osQueue queue, void * value, uint32_t timeout) {
	return xQueueSend(queue, value, timeout);
}

int osQueueReceive(osQueue queue, void * buf, uint32_t timeout){
    return xQueueReceive(queue, buf, timeout);
}