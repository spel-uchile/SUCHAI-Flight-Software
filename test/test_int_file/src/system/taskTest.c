//
// Created by lurrea on 28-10-21.
//
#include "app/system/taskTest.h"

static const char* tag = "file_test_task";
static char current_dir[127] = CURRENT_DIR;

void taskTest(void *params){

    // files names
    char smallText[32] = "text";
    char bigText[32] = "alice29";
    char textExt[32] = ".txt";
    char smallImage[32] = "house";
    char smallImageExt[32] = ".jpg";
    char bigImage[32] = "image";
    char bigImageExt[32] = ".jpeg";
    // variable for tm_send_file
    int node = 3;
    // variables for tm_send_file_part
    int start_frame = 0;
    int end_frame = -1;

    char fname[64];
    strcat(fname, "/");
    strcat(fname, smallText);
    strcat(fname, textExt);
    char filename[192];
    strcat(filename, current_dir);
    strcat(filename, fname);

    char buf[256];
    snprintf(buf, 256, "tm_send_file %s %d", filename, node);
    cmd_t *cmd = cmd_build_from_str(buf);
    cmd_send(cmd);

    char image[64];
    strcat(image, "/");
    strcat(image, smallImage);
    strcat(image, smallImageExt);
    char imagename[192];
    strcat(imagename, current_dir);
    strcat(imagename, image);

    char buf2[256];
    snprintf(buf2, 256, "tm_send_file %s %d", imagename, node);
    cmd = cmd_build_from_str(buf2);
    cmd_send(cmd);

    char buf3[256];
    int file_id = 1;
    snprintf(buf3, 256, "tm_send_file_part %s %d %d %d %d", filename, file_id, start_frame, end_frame, node);
    cmd = cmd_build_from_str(buf3);
    cmd_send(cmd);

    char buf4[256];
    int image_id = 2;
    snprintf(buf4, 256, "tm_send_file_part %s %d %d %d %d", imagename, image_id, start_frame, end_frame, node);
    cmd = cmd_build_from_str(buf4);
    cmd_send(cmd);

    /*
     * TODO ee why it takes to long to send the data

    char fname2[64];
    strcat(fname2, "/");
    strcat(fname2, bigText);
    strcat(fname2, textExt);
    char filename2[192];
    strcat(filename2, current_dir);
    strcat(filename2, fname2);

    char buf5[256];
    snprintf(buf5, 256, "tm_send_file %s %d", filename2, node);
    cmd = cmd_build_from_str(buf5);
    cmd_send(cmd);

    char image2[64];
    strcat(image2, "/");
    strcat(image2, bigImage);
    strcat(image2, bigImageExt);
    char imagename2[192];
    strcat(imagename2, current_dir);
    strcat(imagename2, image2);

    char buf6[256];
    snprintf(buf6, 256, "tm_send_file %s %d", imagename2, node);
    cmd = cmd_build_from_str(buf6);
    cmd_send(cmd);

    char buf7[256];
    int file2_id = 3;
    snprintf(buf7, 256, "tm_send_file_part %s %d %d %d %d", filename2, file2_id, start_frame, end_frame, node);
    cmd = cmd_build_from_str(buf7);
    cmd_send(cmd);

    char buf8[256];
    int image2_id = 4;
    snprintf(buf8, 256, "tm_send_file_part %s %d %d %d %d", imagename2, image2_id, start_frame, end_frame, node);
    cmd = cmd_build_from_str(buf8);
    cmd_send(cmd);
    */

    // Here need a condition to check all data was sent
    // Wait until all data was sent
    sleep(30);

    char received_text[192];
    strcat(received_text, current_dir);
    strcat(received_text, "/recv_files");
    strcat(received_text, fname);

    char buf_cmp1[256];
    snprintf(buf_cmp1, 256, "file_cmp %s %s", filename, received_text);
    cmd = cmd_build_from_str(buf_cmp1);
    cmd_send(cmd);

    char received_image[192];
    strcat(received_image, current_dir);
    strcat(received_image, "/recv_files");
    strcat(received_image, image);

    char buf_cmp2[256];
    snprintf(buf_cmp2, 256, "file_cmp %s %s", imagename, received_image);
    cmd = cmd_build_from_str(buf_cmp2);
    cmd_send(cmd);

    char received_file_text[64];
    strcat(received_file_text, smallText);
    strcat(received_file_text, "-part");
    strcat(received_file_text, textExt);
    char received_text_parts[192];
    strcat(received_text_parts, current_dir);
    strcat(received_text_parts, "/recv_files/");
    strcat(received_text_parts, received_file_text);

    char buf9[256];
    snprintf(buf9, 256, "tm_merge_file %s %d", received_file_text, file_id);
    cmd = cmd_build_from_str(buf9);
    cmd_send(cmd);

    char buf_cmp3[256];
    snprintf(buf_cmp3, 256, "file_cmp %s %s", filename, received_text_parts);
    cmd = cmd_build_from_str(buf_cmp3);
    cmd_send(cmd);

    char received_file_image[64];
    strcat(received_file_image, smallImage);
    strcat(received_file_image, "-part");
    strcat(received_file_image, smallImageExt);
    char received_image_parts[192];
    strcat(received_image_parts, current_dir);
    strcat(received_image_parts, "/recv_files/");
    strcat(received_image_parts, received_file_image);

    char buf10[256];
    snprintf(buf10, 256, "tm_merge_file %s %d", received_file_image, image_id);
    cmd = cmd_build_from_str(buf10);
    cmd_send(cmd);

    char buf_cmp4[256];
    snprintf(buf_cmp4, 256, "file_cmp %s %s", imagename, received_image_parts);
    cmd = cmd_build_from_str(buf_cmp4);
    cmd_send(cmd);

    /*
     * TODO how to check the large file was totally merged
    char received_text2[192];
    strcat(received_text2, current_dir);
    strcat(received_text2, "/recv_files");
    strcat(received_text2, fname2);

    char buf_cmp5[256];
    snprintf(buf_cmp5, 256, "file_cmp %s %s", filename2, received_text2);
    LOGI(tag, buf_cmp5);
    cmd = cmd_build_from_str(buf_cmp5);
    cmd_send(cmd);

    char received_image2[192];
    strcat(received_image2, current_dir);
    strcat(received_image2, "/recv_files");
    strcat(received_image2, image2);

    char buf_cmp6[256];
    snprintf(buf_cmp6, 256, "file_cmp %s %s", imagename2, received_image2);
    cmd = cmd_build_from_str(buf_cmp6);
    cmd_send(cmd);

    strcat(received_text_parts2, bigText);
    strcat(received_text_parts2, "-part");
    strcat(received_text_parts2, textExt);
    char received_text_parts2[192];
    strcat(received_text_parts2, current_dir);
    strcat(received_text_parts2, "/recv_files");
    strcat(received_text_parts2, bigText);

    char buf11[256];
    snprintf(buf11, 256, "tm_merge_file %s %d", received_text_parts2, file2_id);
    cmd = cmd_build_from_str(buf11);
    cmd_send(cmd);

    char buf_cmp7[256];
    snprintf(buf_cmp7, 256, "file_cmp %s %s", filename2, received_text_parts2);
    LOGI(tag, buf_cmp7);
    cmd = cmd_build_from_str(buf_cmp7);
    cmd_send(cmd);

    char received_image_parts2[192];
    strcat(received_image_parts2, current_dir);
    strcat(received_image_parts2, "/recv_files");
    strcat(received_image_parts2, bigImage);
    strcat(received_image_parts2, "-part");
    strcat(received_image_parts2, bigImageExt);

    char buf12[256];
    snprintf(buf12, 256, "tm_merge_file %s %d", received_image_parts2, image2_id);
    cmd = cmd_build_from_str(buf12);
    cmd_send(cmd);

    char buf_cmp8[256];
    snprintf(buf_cmp8, 256, "file_cmp %s %s", imagename2, received_image_parts2);
    cmd = cmd_build_from_str(buf_cmp8);
    cmd_send(cmd);
    */

    // TODO delete all files created
    LOGI(tag, "Deleting all files created")
    char filesPath[192];
    strcat(filesPath, current_dir);
    strcat(filesPath, "/recv_files");
    char buf13[256];
    snprintf(buf13, 256, "file_del %s", filesPath);
    cmd = cmd_build_from_str(buf13);
    cmd_send(cmd);

    cmd = cmd_build_from_str("obc_reset 1010");
    cmd_send(cmd);
}
