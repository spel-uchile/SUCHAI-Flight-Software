//
// Created by lurrea on 25-11-21.
//
#include "app/system/cmdFile.h"
static const char* tag = "cmdFile";

void cmd_file_init(void){
    cmd_add("file_cmp", file_cmp, "%s %s", 2);
    cmd_add("file_del", file_delete, "%s", 1);
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
    //TODO check both files exists

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

int _unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);

    if (rv)
        perror(fpath);

    return rv;
}

int file_delete(char *fmt, char *params, int nparams){

    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_SYNTAX_ERROR;
    }

    char *path;
    if(sscanf(params, fmt, path) != nparams)
    {
        LOGE(tag, "distinct number of params");
        return CMD_SYNTAX_ERROR;
    }
    int rc = nftw(path, _unlink_cb, 64, FTW_DEPTH | FTW_PHYS);

    return rc == 0 ? CMD_OK : CMD_ERROR;
}