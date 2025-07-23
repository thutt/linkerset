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
#include <string.h>
#include <sys/param.h>

#include "module_init.h"

int main(void)
{
    module_init_handle_t handle;
    module_init_handle_t *ih = &handle;

    module_handle_initialize(ih, LINKERSET_SIZE(module_init_info, unsigned));
    if (ih->init_state != IR_MEMORY) {
        topological_sort_modules(ih);
        switch (ih->init_state) {
        case IR_SUCCESS: {
            /* ih->table now contains a set of modules that is in
             * an order suitable for sequential initialization.
             *
             * The original linkerset is unchanged.
             */
            unsigned max_mname = strlen("Module");
            unsigned max_iname = strlen("Initialization");
            unsigned max_fname = strlen("Finalization");

            ih->table_index = 0;
            while (ih->table_index < ih->table_size) {
                max_mname = MAX(max_mname,
                                strlen(ih->table[ih->table_index]->module_name));
                max_iname = MAX(max_iname,
                                strlen(ih->table[ih->table_index]->init_fn_name));
                max_fname = MAX(max_fname,
                                strlen(ih->table[ih->table_index]->fina_fn_name));
                ++ih->table_index;
            }

            assert(ih->table_index == ih->table_size);

            printf("%-*s  %-*s  %-*s\n",
                   max_mname, "Module",
                   max_iname, "Initialization",
                   max_fname, "Finalization");


            ih->table_index = 0;
            while (ih->table_index < ih->table_size) {
                printf("%-*s  %-*s  %-*s\n",
                       max_mname, ih->table[ih->table_index]->module_name,
                       max_iname, ih->table[ih->table_index]->init_fn_name,
                       max_fname, ih->table[ih->table_index]->fina_fn_name);
                ++ih->table_index;
            }

            assert(ih->table_index == ih->table_size);
            break;
        }

        case IR_CYCLE: {
            unsigned i = 0;

            /* Cycles are detected before any module is initialized. */
            printf("Error, cycle detected involving:\n");
            do {
                printf("  %s\n", ih->table[i]->module_name);
                i++;
            } while (i < ih->table_index);
            printf("\n");
            break;
        }

        case IR_FAILED:
            printf("Module '%s' failed to initialized\n",
                   ih->table[ih->table_index]->module_name);
            break;
        }
    }

    module_handle_finalize(ih);
    return 0;
}
