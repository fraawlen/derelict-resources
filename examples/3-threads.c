/**
 * Copyright © 2024 Fraawlen <fraawlen@posteo.net>
 *
 * This file is part of the Derelict Resources (DR) library.
 *
 * This library is free software; you can redistribute it and/or modify it either under the terms of the GNU
 * Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the
 * License or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.
 * See the LGPL for the specific language governing rights and limitations.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this program. If not,
 * see <http://www.gnu.org/licenses/>.
 */

/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <derelict/dr.h>

#include "config.h" /* generated by makefile */

/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

#define _SAMPLE_CONFIG_PATH "/tmp/dr_config_sample"
#define _N_SIMULATIONS       10

/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

static void  _generate_source   (void);
static void *_simulation_thread (void *param);

/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

/**
 * In this 3rd example, a multithreaded software simulator is mimicked. Each simulation, sitting in its own
 * thread, requires the same kind of variables to work with (in this example, the coordinates of something).
 * However, each simulation instance also requires randomized coordinate values to create a different output.
 *
 * In this scenario, each simulation thread will set up its own configuration and seed it with a different
 * value. Thanks to that, during parsing, 'RAND' tokens will be substituted to a different value each time.
 * It should be noted that the parser opens the source files exclusively in read-only mode, therefore
 * multithreaded access to the same source is safe.
 */

int
main(void)
{
	pthread_t threads[_N_SIMULATIONS];

	unsigned long long ids[_N_SIMULATIONS];

	/* init */

	_generate_source();

	/* run pseudo-simulations */

	for (size_t i = 0; i < _N_SIMULATIONS; i++)
	{
		ids[i] = i;
		pthread_create(threads + i, NULL, _simulation_thread, ids + i);
	}
	
	for (size_t i = 0; i < _N_SIMULATIONS; i++)
	{
		pthread_join(threads[i], NULL);
	}

	return 0;
}

/************************************************************************************************************/
/* _ ********************************************************************************************************/
/************************************************************************************************************/

static void
_generate_source(void)
{
	FILE *f;

	if (!(f = fopen(_SAMPLE_CONFIG_PATH, "w")))
	{
		printf("sample configuration in %s could not be generated\n", _SAMPLE_CONFIG_PATH);
		return;
	}

	fprintf(f, "%.*s", config_len, config);

	fclose(f);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

static void *
_simulation_thread(void *param)
{
	dr_config_t *cfg;

	unsigned long coords[3] = {0};
	unsigned long long id;

	/* simulation config setup */

	cfg = dr_config_create(0);
	id  = *(unsigned long long*)param;

	dr_config_push_source(cfg, _SAMPLE_CONFIG_PATH);
	dr_config_seed(cfg, id);
	dr_config_load(cfg);

	if (!dr_config_load(cfg))
	{
		printf("configuration for simulation %llu failed to load\n", id);
	}

	dr_resource_fetch(cfg, "example-3", "coordinates");
	for (size_t i = 0; i < 3 && dr_resource_pick_next_value(cfg); i++)
	{
		coords[i] = dr_resource_convert_to_ulong(cfg);
	}

	/* simulation algorithm */

	printf(
		"simulation %llu -> x = %lu, y = %lu, z = %lu\n",
		id,
		coords[0],
		coords[1],
		coords[2]);

	/* simulation end */

	dr_config_destroy(&cfg);

	pthread_exit(NULL);
}