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
#if !defined(DATATYPES_H_)
#define DATATYPES_H_
#include "symbol_info.h"

DECLARE_SYMINTF_LINKERSETS;

#if defined(ITTYBITTY)
#define N_BITS 8
#else
#define N_BITS 31
#endif
SYMINTF_CPP_INT_ADD(N_BITS);

#define ON 1
SYMINTF_CPP_INT_ADD(ON);

#define OFF 0
SYMINTF_CPP_INT_ADD(OFF);


#if defined(FNORD)
#define GUNGLA 200
#else
#define GUNGLA 151
#endif
SYMINTF_CPP_INT_ADD(GUNGLA);


typedef enum enum_t {
#if defined(IMPORTED_FRUIT)
    kiwi,
    dragon,
#endif
    apple,
    orange,
    pear,
    N_FRUITS
} enum_t;

#if defined(IMPORTED_FRUIT)
SYMINTF_ENUM_ADD(enum_t, kiwi);
SYMINTF_ENUM_ADD(enum_t, dragon);
#endif
SYMINTF_ENUM_ADD(enum_t, apple);
SYMINTF_ENUM_ADD(enum_t, orange);
SYMINTF_ENUM_ADD(enum_t, pear);
SYMINTF_ENUM_ADD(enum_t, N_FRUITS);

typedef struct list_t {
    char           name[32];
    unsigned       size;
#if defined(DOUBLY_LINKED)
    struct list_t *prev;
#endif
    struct list_t *next;
} list_t;

SYMINTF_STRUCT_FIELD_ADD(list_t, name);
SYMINTF_STRUCT_FIELD_ADD(list_t, size);
#if defined(DOUBLY_LINKED)
SYMINTF_STRUCT_FIELD_ADD(list_t, prev);
#endif
SYMINTF_STRUCT_FIELD_ADD(list_t, next);
SYMINTF_SYMBOL_SIZE_ADD(list_t);
#endif
