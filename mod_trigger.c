/* 
* License:
* Copyright 2016 - Jérémie Galarneau <jeremie.galarneau@efficios.com>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Module Name: mod_trigger
*
* Purpose: Invoke a bash script when a request takes too long.
*
*/

#include <apr_strings.h>
#include <apr_base64.h>
#include <apr_lib.h>
#include <unistd.h>

#include "httpd.h"
#include "http_config.h"
#include "http_connection.h"
#include "http_protocol.h"
#include "http_request.h"

#define SCRIPT_PATH "/home/efficios/lttng/snapshot.sh"
#define MAX_REQUEST_TIME ((double) 1.0)

module AP_MODULE_DECLARE_DATA trigger_module;
struct timeval tv_start;

static double get_time_elapsed(struct timeval *start, struct timeval *end)
{
	double a, b;

	b = start->tv_sec + (double) start->tv_usec * (double) 1e-6;
	a = end->tv_sec + (double) end->tv_usec * (double) 1e-6;
	return (a - b);
}

static void set_timeval(struct timeval *tv)
{
	gettimeofday(tv, NULL);
}

static int handle_post_read_request(request_rec *r)
{
	set_timeval(&tv_start);
	return DECLINED;
}

static void trigger(double time_elapsed)
{
	pid_t pid;
	int status;

	/* Double fork so child processes are inherited by init. */
	pid = fork();
	if (pid == 0) {
		pid = fork();
		if (pid == 0) {
			char *args[] = { 0 };
			execv(SCRIPT_PATH, args);
		} else {
			exit(0);
		}
	} else {
		waitpid(pid, &status, 0);
	}
}

static int handle_log_transaction(request_rec *r)
{
	double time_elapsed;
        struct timeval tv_end;

	set_timeval(&tv_end);
	time_elapsed = get_time_elapsed(&tv_start, &tv_end);

	if (time_elapsed > MAX_REQUEST_TIME) {
		trigger(time_elapsed);
	}

	return DECLINED;
}

void trigger_register_hooks(apr_pool_t *p)
{
	ap_hook_log_transaction(handle_log_transaction, NULL, NULL,
			APR_HOOK_FIRST);
	ap_hook_post_read_request(handle_post_read_request, NULL, NULL,
			APR_HOOK_MIDDLE);
}

module AP_MODULE_DECLARE_DATA lttng_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    trigger_register_hooks
};
