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
#include <string.h>
#include "zenoh.h"

int main(int argc, char **argv)
{
    z_init_logger();

    char *expr = "/demo/example/zenoh-c-put";
    if (argc > 1)
    {
        expr = argv[1];
    }
    char *value = "Put from C!";
    if (argc > 2)
    {
        value = argv[2];
    }
    z_owned_config_t config = z_config_default();
    if (argc > 3)
    {
        if (z_config_insert_json(z_loan(config), Z_CONFIG_CONNECT_KEY, argv[3]))
        {
            printf("Couldn't insert value `%s` in configuration at `%s`. This is likely because `%s` expects a JSON-serialized list of strings\n", argv[3], Z_CONFIG_CONNECT_KEY, Z_CONFIG_CONNECT_KEY);
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

    printf("Putting Data ('%s': '%s')...\n", expr, value);
    z_put(z_loan(s), z_expr(expr), (const uint8_t *)value, strlen(value));
    z_close(z_move(s));
    return 0;
}