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
#include "include/cmdTest.h"

static const char* tag = "cmdTest";

void cmd_test_init(void)
{
    cmd_add("test_mult_exe", test_mult_exe, "%d ", 1);
}

int test_mult_exe(char* fmt, char* params, int nparams)
{
    int sec_sleep;

    if(sscanf(params, fmt, &sec_sleep) == nparams)
    {
        int rc = test_sleep(sec_sleep);

        if(rc==0)
            return CMD_OK;
        else
            return CMD_FAIL;
    }
    else
    {
        LOGW(tag, "test_mult_exe used with invalid params: %s", params);
        return CMD_FAIL;
    }
}

