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

/**
 * Lista de comandos disponibles.
 */
typedef enum{
    con_id_help=((unsigned int)(CMD_CON)*0x100), ///< @cmd_first
    con_id_promt, ///< @cmd
    con_id_error_cmd_toolong, ///< @cmd
    con_id_debug_msg, ///< @cmd
    con_id_error_unknown_cmd, ///< @cmd
    con_id_error_invalid_arg, ///< @cmd
    con_id_error_count_arg, ///< @cmd

    //*********************
    con_id_last_one    //Elemento sin sentido, solo se utiliza para marcar el largo del arreglo
}CON_CmdIndx;

#define CON_NCMD ((unsigned char)con_id_last_one)

void con_onResetCmdCON(void);


int con_error_cmd_toolong(char *fmt, char *params, int nparams);
int con_error_count_arg(char *fmt, char *params, int nparams);
int con_error_invalid_arg(char *fmt, char *params, int nparams);
int con_error_unknown_cmd(char *fmt, char *params, int nparams);
int con_debug_msg(char *fmt, char *params, int nparams);
int con_help(char *fmt, char *params, int nparams);

int con_promt(char *fmt, char *params, int nparams);

#endif /* CMD_CONSOLE_H */
