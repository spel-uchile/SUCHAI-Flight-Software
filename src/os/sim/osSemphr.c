/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Ignacio Ibanez Aliaga, ignacio.ibanez@usach.cl
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

#include "osSemphr.h"

int osSemaphoreCreate(osSemaphore* mutex)
{
	if (pthread_mutex_init(mutex, NULL) == 0)
	{
		return CSP_SEMAPHORE_OK;
	} else
	{
		return CSP_SEMAPHORE_ERROR;
	}
}

int osSemaphoreTake(osSemaphore *mutex, uint32_t timeout)
{
	int ret;
	struct timespec ts;
	uint32_t sec, nsec;

	//csp_log_lock("Wait: %p timeout PRIu32\r\n", mutex, timeout);

	if (timeout == portMAX_DELAY)
	{
		ret = pthread_mutex_lock(mutex);
	}
	else
	{
		if (clock_gettime(CLOCK_REALTIME, &ts))
			return CSP_SEMAPHORE_ERROR;

		sec = timeout / 1000;
		nsec = (timeout - 1000 * sec) * 1000000;

		ts.tv_sec += sec;

		if (ts.tv_nsec + nsec >= 1000000000)
			ts.tv_sec++;

		ts.tv_nsec = (ts.tv_nsec + nsec) % 1000000000;

		ret = pthread_mutex_timedlock(mutex, &ts);
	}

	if (ret != 0)
		return CSP_SEMAPHORE_ERROR;

	return CSP_SEMAPHORE_OK;
}

int osSemaphoreGiven(osSemaphore *mutex)
{
	if (pthread_mutex_unlock(mutex) == 0)
	{
		return CSP_SEMAPHORE_OK;
	}
	else
	{
		return CSP_SEMAPHORE_ERROR;
	}
}