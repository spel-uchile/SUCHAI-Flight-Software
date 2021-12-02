//
// Created by lurrea on 25-11-21.
//
#include "suchai/config.h"
#include "suchai/repoData.h"
#include "suchai/repoCommand.h"
#include "suchai/config.h"
#include <ftw.h>

#include "suchai/osDelay.h"

void cmd_file_init(void);

/**
 * Compare two files, comparing each char
 *
 * @param fmt %s %s
 * @param params "<filename1> <filename2>"
 * @param nparams 2
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int file_cmp(char *fmt, char *params, int nparams);

/**
 * Delete all files created
 *
 * @param fmt %s
 * @param params "<path>"
 * @param nparams 1
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int file_delete(char *fmt, char *params, int nparams);

int _unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);