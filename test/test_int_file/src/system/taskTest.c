//
// Created by lurrea on 28-10-21.
//
#include "app/system/taskTest.h"

static const char* tag = "file_test_task";
static char current_dir[127] = CURRENT_DIR;

void taskTest(void *params){

    int node = 3;

    char fname[64] = "/text.txt";
    char filename[192];
    strcat(filename, current_dir);
    strcat(filename, fname);

    char buf[256];
    snprintf(buf, 256, "tm_send_file %s %d", filename, node);
    cmd_t *cmd = cmd_build_from_str(buf);
    cmd_send(cmd);

    /*
     * TODO use small file and see why it takes to long to send the data
    char image[64] = "/image.jpeg";
    char imagename[192];
    strcat(imagename, current_dir);
    strcat(imagename, image);

    char buf2[256];
    snprintf(buf2, 256, "tm_send_file %s %d", imagename, node);
    cmd = cmd_build_from_str(buf2);
    cmd_send(cmd);

    char fname2[64] = "/alice.txt";
    char filename_part[192];
    strcat(filename_part, current_dir);
    strcat(filename_part, fname2);


    char buf3[256];
    int file_id = 1;
    int start_frame = 0;
    int end_frame = -1;
    snprintf(buf3, 256, "tm_send_file_part %s %d %d %d %d", filename_part, file_id, start_frame, end_frame, node);
    cmd = cmd_build_from_str(buf3);
    cmd_send(cmd);
    */

    // Here need a condition to check all data was sent
    // Wait until all data was sent
    sleep(10);

    char received_text[192];
    strcat(received_text, current_dir);
    strcat(received_text, "/recv_files");
    strcat(received_text, fname);

    char buf_cmp1[256];
    snprintf(buf_cmp1, 256, "file_cmp %s %s", filename, received_text);
    LOGI(tag, buf_cmp1);
    cmd = cmd_build_from_str(buf_cmp1);
    cmd_send(cmd);

    /*
     * TODO how to check the large file was totally merged
    char received_image[192];
    strcat(received_image, current_dir);
    strcat(received_image, "/recv_files");
    strcat(received_image, image);

    char buf_cmp2[256];
    snprintf(buf_cmp2, 256, "file_cmp %s %s", imagename, received_image);
    cmd = cmd_build_from_str(buf_cmp2);
    cmd_send(cmd);

    char received_text2[192];
    strcat(received_text2, current_dir);
    strcat(received_text2, "/recv_files");
    strcat(received_text2, fname2);

    char buf4[256];
    snprintf(buf4, 256, "tm_merge_file %s %d", fname2, file_id);
    cmd = cmd_build_from_str(buf4);
    cmd_send(cmd);

    char buf_cmp3[256];
    snprintf(buf_cmp3, 256, "file_cmp %s %s", filename_part, received_text2);
    cmd = cmd_build_from_str(buf_cmp3);
    cmd_send(cmd);
    */

    // TODO delete all files created

    osTaskDelete(NULL);
}
