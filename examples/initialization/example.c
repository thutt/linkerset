/* BSD 2-Clause License
 *
 * Copyright (c) 2025 Logic Magicians Software (Taylor Hutt)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>

#include "module_init.h"

int main(void)
{
    module_init_handle_t handle;

    printf("*** Initializing modules.\n");
    module_initialization(&handle);

    switch (handle.init_state) {
    case IR_SUCCESS:
        break;

    case IR_CYCLE: {
        unsigned i = 0;

        /* Cycles are detected before any module is initialized. */
        printf("Error, cycle detected involving:\n");
        do {
            printf("  %s\n", handle.table[i]->module_name);
            i++;
        } while (i < handle.table_index);
        printf("\n");
        break;
    }

    case IR_FAILED:
        printf("Module '%s' failed to initialized\n",
               handle.table[handle.table_index]->module_name);
        break;
    }

    switch (handle.init_state) {
    case IR_SUCCESS: /* Program initialized successfully. */
    case IR_FAILED:  /* Some modules initialized. */
        printf("\n\n*** Finalizing modules.\n");
        module_finalization(&handle);
        break;
    }

    return 0;
}
