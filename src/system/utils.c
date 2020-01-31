/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2019, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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
 
#include "utils.h"

osSemaphore log_mutex;  ///< Sync logging functions, require initialization
void (*log_function)(const char *lvl, const char *tag, const char *msg, ...);
log_level_t log_lvl;
uint8_t log_node;

void log_print(const char *lvl, const char *tag, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    fprintf(LOGOUT,"[%s][%lu][%s] ", lvl, (unsigned long)time(NULL), tag);
    vfprintf(LOGOUT, msg, args);
    fprintf(LOGOUT,CRLF); fflush(LOGOUT);
    va_end(args);
}

void log_send(const char *lvl, const char *tag, const char *msg, ...)
{
    // Create a packet for the log message
    csp_packet_t *packet = csp_buffer_get(SCH_BUFF_MAX_LEN);
    if(packet == NULL)
        return;

    // Format message with variadic arguments
    va_list args;
    va_start(args, msg);
    int len = vsnprintf((char *)(packet->data), SCH_BUFF_MAX_LEN, msg, args);
    va_end(args);

    // Make sure its is a null terminating string
    packet->length = len;
    packet->data[len] = '\0';

    // Sending message without connection nor reply.
    int rc = csp_sendto(CSP_PRIO_NORM, (uint8_t)log_node, SCH_TRX_PORT_DBG,
                        SCH_TRX_PORT_DBG, CSP_O_NONE, packet, 100);
    if(rc != 0)
        csp_buffer_free((void *)packet);
}

void log_set(log_level_t level, int node)
{
    osSemaphoreTake(&log_mutex, portMAX_DELAY);
    log_lvl = level;
    log_node = (uint8_t)node;
    log_function = node > 0 ? log_send : log_print;
    osSemaphoreGiven(&log_mutex);
}

int log_init(log_level_t level, int node)
{
    int rc = osSemaphoreCreate(&log_mutex);
    log_set(level, node);
    return rc;
}