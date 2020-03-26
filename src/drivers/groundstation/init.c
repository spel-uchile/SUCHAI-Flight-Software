/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Matias Ramirez Martinez, nicoram.mt@gmail.com
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

#include "init.h"

static const char *tag = "on_reset";

void on_close(int signal)
{
    dat_repo_close();
    cmd_repo_close();

    LOGI(tag, "Exit system!");
    exit(signal);
}

void on_reset(void)
{
    /* Register INT/TERM signals */
    struct sigaction act;
    act.sa_handler = on_close;
    sigaction(SIGINT, &act, NULL);  // Register CTR+C signal handler
    sigaction(SIGTERM, &act, NULL);
}
