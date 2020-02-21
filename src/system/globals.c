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

#include "globals.h"

osQueue dispatcher_queue;         ///< Commands queue
osQueue executer_cmd_queue;       ///< Executer commands queue
osQueue executer_stat_queue;      ///< Executer result queue
osSemaphore repo_data_sem;        ///< Data repository mutex
osSemaphore repo_data_fp_sem;     ///< Flight plan repository mutex
osSemaphore repo_machine_sem;     ///< State machine repository mutex
osSemaphore repo_cmd_sem;         ///< Command repository mutex
