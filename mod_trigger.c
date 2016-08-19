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

#include "httpd.h"
#include "http_config.h"
#include "http_connection.h"
#include "http_protocol.h"
#include "http_request.h"

#define SCRIPT_PATH "/home/efficios/lttng/snapshot.sh"

module AP_MODULE_DECLARE_DATA trigger_module;

static int log_slow_log_transaction(request_rec *r)
{
	return DECLINED;
}

void trigger_register_hooks(apr_pool_t *p)
{
    ap_hook_log_transaction(handle_log_transaction, NULL, NULL, APR_HOOK_FIRST);
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