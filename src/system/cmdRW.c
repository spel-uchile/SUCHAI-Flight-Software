/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *      Copyright 2018, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2018, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2018, Camilo Rojas, camrojas@uchile.cl
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

#include "cmdRW.h"

static const char* tag = "cmdRW";

void cmd_rw_init(void)
{
    cmd_add("rw_get_speed", rw_get_speed, "", 0);
    cmd_add("rw_get_current", rw_get_current, "", 0);
    cmd_add("rw_set_speed", rw_set_speed, "%d %d", 2);
}

int rw_get_speed(char *fmt, char *params, int nparams)
{
    LOGI(tag, "Getting all speeds");

    uint16_t speed1 = rwdrv10987_get_speed(MOTOR1_ID);
    uint16_t speed2 = rwdrv10987_get_speed(MOTOR2_ID);
    uint16_t speed3 = rwdrv10987_get_speed(MOTOR3_ID);

    LOGI(tag, "Sampled speed1: %d, speed2: %d, speed3: %d", speed1, speed2, speed3);

    return 1; //TO DO: check error code
}

int rw_get_current(char *fmt, char *params, int nparams)
{
    LOGI(tag, "Sampling all currents");

    float current1 = rwdrv10987_get_current(MOTOR1_ID); //[mA]
    float current2 = rwdrv10987_get_current(MOTOR2_ID); //[mA]
    float current3 = rwdrv10987_get_current(MOTOR3_ID); //[mA]

    LOGI(tag, "Sampled current1: %f, current2: %f, current3: %f", current1, current2, current3);

    return 1; //TO DO: check error code
}

int rw_set_speed(char *fmt, char *params, int nparams)
{
    LOGI(tag, "Speed command");

    int motor_id;
    int speed;

    if(sscanf(params, fmt, &motor_id, &speed) == nparams)
    {
        LOGI(tag, "Setting motor: %d speed: %d", motor_id, speed);

        rwdrv10987_set_speed(motor_id, speed);
    }
    return 1; //TO DO: check error code
}
