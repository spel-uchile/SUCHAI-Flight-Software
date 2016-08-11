/*                            SERIAL CONSOLE
 *                              For PIC24F
 * 
 *      Copyright 2012, Francisco Reyes Aspe, komodotas@gmail.com
 *      Copyright 2012, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CONSOLE_H
#define CONSOLE_H

#include <string.h>
#include <stdlib.h>

#include "rs232_suchai.h"
#include "DebugIncludes.h"  //para con_printf

#include "cmdRepository.h"

#define CON_BUF_WIDTH 32
#define CON_ARG_WIDTH 8

#define CON_ARG_QTY 8

void con_init(void);
void con_char_handler(char newchar);
void con_cmd_from_entry(char *entry);
DispCmd con_cmd_handler(void);

BOOL con_set_active(BOOL on_off);
BOOL StrIsInt(char *str);

#endif //CONSOLE_H
