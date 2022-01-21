/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2021, Camilo Rojas Milla, camrojas@uchile.cl
 *      Copyright 2021, Gustavo Diaz Huenupan, gustavo.diaz@ing.uchile.cl
 *      Copyright 2021, Elias Obreque Sepulveda, elias.obreque@uchile.cl
 *      Copyright 2021, Javier Morales Rodriguez, javiermoralesr95@gmail.com
 *      Copyright 2021, Luis Jimenez Verdugo, luis.jimenez@ing.uchile.cl
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
#include "app/system/cmdTLE.h"
//#include "app/system/cmdADCS.h"

//static const char* tag = "cmdADCS";
static const char* tag = "cmdTLE";

#define ADCS_PORT 7
#define TLE_BUFF_LEN 70

TLE tle;
static char tle1[TLE_BUFF_LEN]; //"1 42788U 17036Z   20054.20928660  .00001463  00000-0  64143-4 0  9996";
static char tle2[TLE_BUFF_LEN]; //"2 42788  97.3188 111.6825 0013081  74.6084 285.6598 15.23469130148339";

void cmd_tle_init(void)
{
    cmd_add("tle_get", tle_get, "", 0);
    cmd_add("tle_set", tle_set, "%d %n", 2);
    cmd_add("tle_update", tle_update, "", 0);
    cmd_add("tle_prop", tle_prop, "%ld", 1);
}

int tle_get(char *fmt, char *params, int nparams)
{
    LOGR(tag, "%s", tle1);
    LOGR(tag, "%s", tle2);
    return CMD_OK;
}

int tle_set(char *fmt, char *params, int nparams)
{
    int line_n, next;

    // fmt: "%d %n", nparams: 2
    // First number is parsed @line_n, but then all the line pointed by @params
    // is copied to the corresponding TLE line. Examples of @params:

    //          1         2         3         4         5         6
    //0123456789012345678901234567890123456789012345678901234567890123456789
    //----------------------------------------------------------------------
    //1 42788U 17036Z   20054.20928660  .00001463  00000-0  64143-4 0  9996
    //2 42788  97.3188 111.6825 0013081  74.6084 285.6598 15.23469130148339
    if(params == NULL || sscanf(params, fmt, &line_n, &next) != nparams-1)
    {
        LOGE(tag, "Error parsing parameters!");
        return CMD_SYNTAX_ERROR;
    }

    if(strlen(params) != 69)
    {
        LOGE(tag, "Invalid TLE line len! (%d)", strlen(params));
        return CMD_SYNTAX_ERROR;
    }

    if(line_n == 1)
    {
        memset(tle1, 0, TLE_BUFF_LEN);
        strncpy(tle1, params, TLE_BUFF_LEN-1);
    }
    else if(line_n == 2)
    {
        memset(tle2, 0, TLE_BUFF_LEN);
        strncpy(tle2, params, TLE_BUFF_LEN-1);
    }
    else
    {
        LOGE(tag, "Invalid TLE Line parameter");
        return CMD_SYNTAX_ERROR;
    }

    return CMD_OK;
}

int tle_update(char *fmt, char *params, int nparams)
{
    parseLines(&tle, tle1, tle2);
    //TODO: Check errors
    if(tle.sgp4Error != 0)
    {
        dat_set_system_var(dat_ads_tle_epoch, 0);
        return CMD_ERROR;
    }

    LOGR(tag, "TLE updated to epoch %.8f (%d)", tle.epoch, (int)(tle.epoch/1000.0));
    dat_set_system_var(dat_ads_tle_epoch, (int)(tle.epoch/1000.0));

    return CMD_OK;
}

int tle_prop(char *fmt, char *params, int nparams)
{
    double r[3];  // Sat position in ECI frame
    double v[3];  // Sat velocity in ECI frame
    int ts=0;

    if(params != NULL && sscanf(params, fmt, &ts) != nparams)
        return CMD_SYNTAX_ERROR;

    if(ts == 0)
        ts = dat_get_time();

    double ts_mili = 1000.0 * (double) ts;

    double diff = (double)ts - (double)tle.epoch/1000.0;
    diff /= 60.0;

    getRVForDate(&tle, ts_mili, r, v);

    LOGD(tag, "T : %.8f - %.8f = %.8f", ts_mili/1000.0, tle.epoch/1000.0, diff);
    LOGD(tag, "R : (%.8f, %.8f, %.8f)", r[0], r[1], r[2]);
    LOGD(tag, "V : (%.8f, %.8f, %.8f)", v[0], v[1], v[2]);
    LOGD(tag, "Er: %d", tle.rec.error);

    if(tle.sgp4Error != 0)
        return CMD_ERROR;

    value32_t pos[3] = {{.f=(float)r[0]},{.f=(float)r[1]}, {.f=(float)r[2]}};
    dat_set_status_var(dat_ads_pos_x, pos[0]);
    dat_set_status_var(dat_ads_pos_y, pos[1]);
    dat_set_status_var(dat_ads_pos_z, pos[2]);
    dat_set_system_var(dat_ads_tle_last, (int)ts);

    return CMD_OK;
}
