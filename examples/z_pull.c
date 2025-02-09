//
// Copyright (c) 2022 ZettaScale Technology
//
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
// which is available at https://www.apache.org/licenses/LICENSE-2.0.
//
// SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
//
// Contributors:
//   ZettaScale Zenoh Team, <zenoh@zettascale.tech>
//
#include <stdio.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#define sleep(x) Sleep(x * 1000)
#else
#include <unistd.h>
#endif
#include "zenoh.h"

void data_handler(const z_sample_t *sample, const void *arg)
{
    printf(">> [Subscriber] Received ('%.*s': '%.*s')\n",
           (int)sample->key.suffix.len, sample->key.suffix.start,
           (int)sample->value.len, sample->value.start);
}

int main(int argc, char **argv)
{
    z_init_logger();

    char *expr = "/demo/example/**";
    if (argc > 1)
    {
        expr = argv[1];
    }
    z_owned_config_t config = z_config_default();
    if (argc > 2)
    {
        if (!z_config_insert_json(z_loan(config), Z_CONFIG_CONNECT_KEY, argv[2]))
        {
            printf("Couldn't insert value `%s` in configuration at `%s`. This is likely because `%s` expects a JSON-serialized list of strings\n", argv[2], Z_CONFIG_CONNECT_KEY, Z_CONFIG_CONNECT_KEY);
            exit(-1);
        }
    }

    printf("Openning session...\n");
    z_owned_session_t s = z_open(z_move(config));
    if (!z_check(s))
    {
        printf("Unable to open session!\n");
        exit(-1);
    }

    printf("Creating Subscriber on '%s'...\n", expr);
    z_subinfo_t subinfo;
    subinfo.reliability = z_reliability_t_RELIABLE;
    subinfo.mode = z_submode_t_PULL;
    subinfo.period = z_period_NONE;
    z_owned_subscriber_t sub = z_subscribe(z_loan(s), z_expr(expr), subinfo, data_handler, NULL);
    if (!z_check(sub))
    {
        printf("Unable to create subscriber.\n");
        exit(-1);
    }

    printf("Press <enter> to pull data...\n");
    char c = 0;
    while (c != 'q')
    {
        c = getchar();
        if (c == -1)
        {
            sleep(1);
        }
        else
        {
            z_pull(&sub);
        }
    }

    z_subscriber_close(z_move(sub));
    z_close(z_move(s));
    return 0;
}