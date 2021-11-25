//
// Created by lurrea on 28-10-21.
//
#include "app/system/taskTest.h"

static const char* tag = "file_test_task";
static char current_dir[127] = CURRENT_DIR;

void taskTest(void *params){

    int node = 3;

    char filename[192];
    snprintf(filename, 192, "%s/text.txt", current_dir);

    char buf[256];
    snprintf(buf, 256, "tm_send_file %s %d", filename, node);
    cmd_t *cmd = cmd_build_from_str(buf);
    cmd_send(cmd);

    char imagename[192];
    snprintf(imagename, 192, "%s/image.jpeg", current_dir);

    char buf2[256];
    snprintf(buf2, 256, "tm_send_file %s %d", filename, node);
    cmd = cmd_build_from_str(buf);
    cmd_send(cmd);

    char filename_part[192];
    snprintf(filename_part, 192, "%s/alice29.txt", current_dir);

    // TODO know how to use tm_send_file_part command
    //char buf3[256];
    //snprintf(buf3, 256, "tm_send_file_part %s %d", filename_part, node);
    //cmd = cmd_build_from_str(buf3);
    //cmd_send(cmd);

    // Wait until all data was sent
    sleep(5);

    // TODO know where are and how retrieve the data sent
    //received_text = current_dir + "bla_bla.txt";
    //char buf_cmp1[256];
    //snprintf(buf_cmp1, 256, "cmp_file %s %s", filename, received_text);
    //cmd = cmd_build_from_str(buf_cmp1);
    //cmd_send(cmd);

    //received_image = current_dir + "bla_bla.txt";
    //char buf_cmp2[256];
    //snprintf(buf_cmp1, 256, "cmp_file %s %s", imagename, received_text);
    //cmd = cmd_build_from_str(buf_cmp2);
    //cmd_send(cmd);

    //TODO receive and merge data


    osTaskDelete(NULL);
}
