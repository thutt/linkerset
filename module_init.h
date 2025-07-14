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
 */
typedef enum initialization_result_t {
    IR_SUCCESS,
    IR_CYCLE,
    IR_FAILED
} initialization_result_t;


/* module_init_fn_t:
 *
 *  This functions as the signature of the module initialization
 *  function.
 */
typedef int (*module_init_fn_t)(void);


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
#define DECLARE_MODULE(mname_, init_fn_)                                \
    typedef module_import_t XCONCAT_(MODULE_IMPORT(mname_),_t);         \
    LINKERSET_DECLARE(MODULE_IMPORT(mname_));                           \
    extern int XCONCAT_(mname_,_init_fn)(void);                         \
    module_init_info_t MODULE_INIT(mname_) = {                          \
        .module_name = XSTRING_(mname_),                                \
        .init_fn       = init_fn_,                                      \
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


/* initialize_module
 *
 *   This function performs the administrative work around invoking
 *   the initialization function of all imported modules, and the
 *   current module.
 */
static inline initialization_result_t
initialize_module(module_init_info_t        *mip,
                  const module_init_info_t **module_err)
{
    if (mip->init_state == IS_INITIALIZING) {
        *module_err = mip;
        return IR_CYCLE;
    } else if (mip->init_state == IS_UNINITIALIZED) {
        initialization_result_t  res;
        int                      init_result;
        void                    *p = mip->imports.start;

        mip->init_state = IS_INITIALIZING;

        /* Initialize imported modules first. */
        while (p < mip->imports.stop) {
            module_init_info_t *impp = *(module_init_info_t **)p;
            res = initialize_module(impp, module_err);
            if (res != IR_SUCCESS) {
                return res;
            }
            p += sizeof(void *);
        }

        mip->init_state = IS_INITIALIZING;
        if (mip->init_fn != NULL) {
            init_result = mip->init_fn();
            if (init_result != 0) {
                *module_err = mip;
                return IR_FAILED;
            }
        }
        mip->init_state = IS_INITIALIZED;
    } else {
        assert(mip->init_state == IS_INITIALIZED);
    }
    return IR_SUCCESS;
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
 *   If the result is IR_SUCCESS, the contents of 'module_err' are
 *   undefined.
 */
static inline initialization_result_t
module_initialization(const module_init_info_t **module_err)
{
    initialization_result_t res;

    LINKERSET_ITERATE(module_init_info, mi, {
            const module_init_info_t *mip = mi;

            res = initialize_module(mi, module_err);
            if (res != IR_SUCCESS) {
                return res;
            }
        });
    return IR_SUCCESS;
}
#endif
