//
// Created by lurrea on 25-11-21.
//
#include "app/system/cmdFile.h"
static const char* tag = "cmdFile";

void cmd_file_init(void){
    cmd_add("file_cmp", file_cmp, "%s %s", 2);
}

int file_cmp(char *fmt, char *params, int nparams){

    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_SYNTAX_ERROR;
    }

    char *filename1;
    char *filename2;
    if(sscanf(params, fmt, filename1, filename2) != nparams)
    {
        LOGE(tag, "distinct number of params");
        return CMD_SYNTAX_ERROR;
    }
    FILE *file1;
    file1 = fopen(filename1, "r");
    FILE *file2;
    file2 = fopen(filename2, "r");

    char ch1, ch2;
    do
    {
        // Input character from both files
        ch1 = fgetc(file1);
        ch2 = fgetc(file2);

        // If characters are not same then return -1
        if (ch1 != ch2)
            return CMD_ERROR;

    } while (ch1 != EOF && ch2 != EOF);


    /* If both files have reached end */
    if (ch1 == EOF && ch2 == EOF)
        return CMD_OK;
    else
        return CMD_ERROR;

}