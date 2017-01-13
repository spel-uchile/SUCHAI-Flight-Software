/**
 * @file  cmdConsole.h
 * @author Tomas Opazo T
 * @author Carlos Gonzalez C
 * @date 2012
 * @copyright GNU Public License.
 *
 * Este header contiene las definiciones de comandos asociados a la consola
 * serial. Mensajes de error especiales relacionado con la entrada y respuesta
 * a comandos por la consola.
 */

#ifndef CMD_CONSOLE_H
#define CMD_CONSOLE_H

#define CMD_CON 0x20 ///< OBC commands group identifier

#include "repoCommand.h"


void cmd_console_init(void);

/* TODO: Move documentation to this header */
int con_error_cmd_toolong(char *fmt, char *params, int nparams);
int con_error_count_arg(char *fmt, char *params, int nparams);
int con_error_invalid_arg(char *fmt, char *params, int nparams);
int con_error_unknown_cmd(char *fmt, char *params, int nparams);
int con_debug_msg(char *fmt, char *params, int nparams);
int con_help(char *fmt, char *params, int nparams);

int con_promt(char *fmt, char *params, int nparams);

#endif /* CMD_CONSOLE_H */
