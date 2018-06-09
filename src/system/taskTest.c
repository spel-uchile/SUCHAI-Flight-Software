/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Matias Ramirez Martinez, nicoram.mt@gmail.com
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
#include "taskTest.h"

static const char *tag = "Test";

void taskTest(void *param)
{
    char* cmd1 = "test_print_int";
    char* cmd2 = "test_print_char";
    char* cmd3 = "test_print_char2";
    cmd_t *test_int = cmd_get_str(cmd1);
    cmd_add_params_str(test_int, "");
    cmd_t *test_char = cmd_get_str(cmd2);
    cmd_add_params_str(test_char, "");
    cmd_t *test_char2 = cmd_get_str(cmd3);
    cmd_add_params_str(test_char2, "");
    cmd_send(test_int);
    cmd_send(test_char);
    cmd_send(test_char2);
}
