//
// Created by franco on 17-11-21.
//

#include "../utils/newRepoData.h"

int get_simple_message(simple_message *message){
    if(message == NULL){
        return -1; //ERROR
    }
    int rc = storage_status_get_value_idx(message->idx, &(message -> data), message->table_name);
    if (rc != 0) return -1; //ERROR
    return 0; //SUCCESS
}

int set_simple_message(simple_message *message){
    if(message == NULL) return -1; //ERROR
    int rc = storage_status_set_value_idx(message->idx, message -> data, message -> table_name);
    if(rc != 0) return -1; //ERROR
    return 0; //SUCCESS
}