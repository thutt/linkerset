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
 *
 *
 * This header uses linkersets to facilitate initializing C modules
 * that ensures the modules are processed in an order that guarantees
 * imported modules are initialized before the importer.  No other
 * other ordering guarantees are made.
 */
#if !defined(MODULE_INIT_H_)
#define MODULE_INIT_H_

#include <assert.h>
#include <malloc.h>
#include "linkerset.h"

/* init_state_t
 *
 *   o IS_UNINITIALIZED
 *
 *     When a module in this state is encountered when initializing
 *     all modules, the internal state will be set to IS_INITIALIZING,
 *     all imported modules will be initialized, then the
 *     initialization function will be invoked.  When the
 *     initialization function returns, the state will be set to
 *     IS_INITIALIZED.
 *
 *   o IS_INITIALIZING
 *
 *     The module is being initialized.  An attempt to initialize this
 *     module again indicates an import cycle.  The initialization
 *     process will terminate with the result IR_CYCLE.
 *
 *   o IS_INITIALIZED
 *
 *     The module is fully initialized.  An attempt to initialize this
 *     module again is a NOP.
 */
typedef enum init_state_t {
    IS_UNINITIALIZED,
    IS_INITIALIZING,
    IS_INITIALIZED
} init_state_t;


/* initialization_result_t
 *
 *  IR_SUCCESS: All modules have been initialized successfully.
 *
 *  IR_CYCLE: An import cycle has been found in the set of modules.
 *
 *  IR_FAILED: An module's initialziation function has returned a
 *             non-zero value to indicate it could not properly
 *             initialize.
 *
 *  IR_MEMORY: ADD COMMENT
 */
typedef enum initialization_result_t {
    IR_SUCCESS,
    IR_CYCLE,
    IR_FAILED,
    IR_MEMORY
} initialization_result_t;


/* module_init_fn_t:
 *
 *  This functions as the signature of the module initialization
 *  function.
 */
typedef int (*module_init_fn_t)(void);


/* module_fina_fn_t:
 *
 *  This functions as the signature of the module finalization
 *  function.
 */
typedef int (*module_fina_fn_t)(void);


/* module_import_info_t
 *
 *   This describes the [start, stop) of the linkerset containing the
 *   imports of a module.  This is used to process the initialization
 *   functions of imported modules.
 *
 *   There will be one linkerset of imports for each module contained
 *   in the program.
 */
typedef struct module_import_info_t {
    void *start;
    void *stop;
} module_import_info_t;


/* module_init_info_t
 *
 *  This linkerset describes the modules declared in the program.  It
 *  is used to fully initialize all declared modules.
 */
typedef struct module_init_info_t {
    const char           *module_name;
    module_import_info_t  imports;
    module_init_fn_t      init_fn;
    module_fina_fn_t      fina_fn;
    init_state_t          init_state;
} module_init_info_t;


/* module_import_t
 *
 *   This type is a linkerset that is used to collate imports of a
 *   module into a single data structure.
 *
 *   Because linkersets are name-based, this type is not used
 *   directly; instead a module-specific alias is created in
 *   DECLARE_MODULE so that each importing module's linkerset of
 *   imports will be unique.
 *
 *   The MODULE_IMPORT macro is used to produce the name of the alias.
 */
typedef struct module_import_t {
    module_init_info_t *importee_init;
} module_import_t;


/* module_init_info
 *
 *  This linkerset is used to hold the initialization information for
 *  all declared modules.
 */
LINKERSET_DECLARE(module_init_info);


/* MODULE_IMPORT
 *
 *  This macro produces an alias name for module_import_t so that each
 *  module's import list will be kept in a unique linkerset.
 */
#define MODULE_IMPORT(mname_) XCONCAT_(mname_,_import_info)


/* MODULE_INIT
 *
 *  This macro produces the name of the module-unique variable that
 *  holds the initialization information that is put into the
 *  'module_init_info' linkerset.
 */
#define MODULE_INIT(mname_) XCONCAT_(mname_,_init_)


/* DECLARE_MODULE
 *
 *  This macro declares a module, and sets up all the necessary
 *  internal state to maintain its initialization information.
 *
 *  NOTE:
 *
 *    Declaring two modules with the same name is results in undefined
 *    behavior.
 *
 *    Using NULL as the value of init_fn_ results in declaring a
 *    module with no initialization function.
 */
#define DECLARE_MODULE(mname_, init_fn_, fina_fn_)                      \
    typedef module_import_t XCONCAT_(MODULE_IMPORT(mname_),_t);         \
    LINKERSET_DECLARE(MODULE_IMPORT(mname_));                           \
    extern int XCONCAT_(mname_,_init_fn)(void);                         \
    module_init_info_t MODULE_INIT(mname_) = {                          \
        .module_name = XSTRING_(mname_),                                \
        .init_fn       = init_fn_,                                      \
        .fina_fn       = fina_fn_,                                      \
        .imports.start = LINKERSET_START(MODULE_IMPORT(mname_)),        \
        .imports.stop  = LINKERSET_STOP(MODULE_IMPORT(mname_)),         \
        .init_state    = IS_UNINITIALIZED                               \
    };                                                                  \
    LINKERSET_ADD_ITEM(module_init_info, XCONCAT_(mname_, _init_))


/* IMPORT
 *
 *   This macro adds an imported module to the set of imports for the
 *   importer.
 */
#define IMPORT(importer_, importee_)                                    \
    extern module_init_info_t MODULE_INIT(importee_);                   \
    LINKERSET_ADD_ITEM(MODULE_IMPORT(importer_), MODULE_INIT(importee_));


/* init_handle_t
 *
 */
typedef struct module_init_handle_t {
    /* init_state:
     *
     *   The state of initialization at the end of the function
     *   receiving this type asd an argument.
     *
     * table_index:
     *
     *   This field is used to index the 'table' field.  When the
     *   entire table has been successfully processed, its value can
     *   be one larger than the last valid index into the table.
     *
     *   inv: 0 <= table_index <= table_size
     *
     * table_size:
     *
     *   This is the number of elements allocated for 'table'.
     *
     * table:
     *
     *   This field is used to topologically sort the set of modules
     *   into an order suitable for sequential initialization.  If the
     *   set of modules cannot be put into an order for sequential
     *   initialization, no modules will be initialized, and
     *   'init_state' will not be set to IR_SUCCESS.
     *
     *   inv:
     *     (Ai: 0 <= i < table_index: table[i] is valid 'module_init_info_t *') &&
     *     (Ai: table_index <= i < table_size: table[i] is undefined)
     */
    initialization_result_t    init_state;
    unsigned                   table_index;
    unsigned                   table_size;
    module_init_info_t       **table;
} module_init_handle_t;


static inline void
initialize_handle(module_init_handle_t *ih, unsigned table_size)
{
    ih->init_state  = IR_SUCCESS;
    ih->table_index = 0;
    ih->table_size  = table_size;
    ih->table       = 0;
    ih->table_index = 0;
    if (table_size != 0) {
        ih->table = calloc(ih->table_size, sizeof(module_init_info_t *));
    }
}


/* topological_sort_modules
 *
 *   This function performs the administrative work around invoking
 *   the initialization function of all imported modules, and the
 *   current module.
 */
static inline void
topological_sort_modules(module_init_handle_t *ih,
                         module_init_info_t   *mip)
{
    if (mip->init_state == IS_INITIALIZING) {
        /* Cycle detected. Store offending module in cycle table. */
        ih->init_state  = IR_CYCLE;
        ih->table[0]    = mip;
        ih->table_index = 1;
    } else if (mip->init_state == IS_UNINITIALIZED) {
        /* Follow imported modules, depth first. */

        initialization_result_t  res;
        void                    *p = mip->imports.start;

        mip->init_state = IS_INITIALIZING; /* For cycle detection. */
        while (p < mip->imports.stop) {
            module_init_info_t *impp = *(module_init_info_t **)p;
            topological_sort_modules(ih, impp);
            if (ih->init_state == IR_CYCLE) {
                ih->table[ih->table_index] = mip;
                ih->table_index++;
                return;
            } else if (ih->init_state != IR_SUCCESS) {
                return;
            }
            p += sizeof(void *);
        }

        /* mip is a module that is clear to be initialized. */
        ih->table[ih->table_index] = mip;
        ++ih->table_index;
        mip->init_state = IS_INITIALIZED;
    } else {
        assert(mip->init_state == IS_INITIALIZED);
    }
}


/* module_initialization
 *
 *   This function processes the module_init_info linkerset, invoking
 *   the initialization functions of all declared modules.
 *
 *   If the result is not IR_SUCCESS, then it indicates the type of
 *   error encountered, and 'module_err' will refer to the module at
 *   which the error occured.
 *
 * XXX fix this comment.  Describe errors.
 *   If the result is IR_SUCCESS, the contents of 'module_err' are
 *   undefined.
 */
static inline void
module_initialization(module_init_handle_t *ih)
{
    initialize_handle(ih, LINKERSET_SIZE(module_init_info, unsigned));

    if (ih->table == NULL) {
        ih->init_state = IR_MEMORY;
        return;
    }

    LINKERSET_ITERATE(module_init_info, mi, {
            topological_sort_modules(ih, mi);
            if (ih->init_state != IR_SUCCESS) {
                return;
            }
        });
    assert(ih->table_index == ih->table_size);

    /* ih->table now contains a set of modules that is in an order
     * suitable for sequential initialization.
     *
     * The original linkerset is unchanged.
     */
    ih->table_index = 0;
    while (ih->table_index < ih->table_size) {
        if (ih->table[ih->table_index]->init_fn != NULL) {
            int init_result;

            ih->table[ih->table_index]->init_state = IS_INITIALIZING;
            init_result= ih->table[ih->table_index]->init_fn();
            if (init_result != 0) {
                ih->init_state = IR_FAILED;
                return;
            }
            ih->table[ih->table_index]->init_state = IS_INITIALIZED;
        }
        ++ih->table_index;
    }

    assert(ih->table_index == ih->table_size);
}


static inline void
module_finalization(module_init_handle_t *ih)
{
    if (ih->table != NULL) {
        /* ih->table != NULL -> finalization not done. */

        do {
            --ih->table_index;

            if (ih->table[ih->table_index]->fina_fn != NULL) {
                int init_result;

                init_result= ih->table[ih->table_index]->fina_fn();
                if (init_result != 0) {
                    /* Finalizing a module failed.  Stop finalizing
                     * lower-level modules because the one that failed
                     * may still be relying on functionality from a
                     * lower-level module.
                     *
                     * At this point, the whole program is in a bad
                     * state.  'ih' is not cleaned up and
                     * re-initialized, as happens in a successful
                     * finalization of the whole program.
                     */
                    ih->init_state = IR_FAILED;
                    return;
                }
            }
        } while (ih->table_index != 0);
        free(ih->table);
        ih->table = NULL;

        /* Re-initialize the handle. */
        initialize_handle(ih, 0);
    }
}
#endif
