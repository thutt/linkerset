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
 * Herein is an implementation of a system that allows one to do the
 * following:
 *
 *  1. Make C symbol information available to Gnu assembler sources.
 *
 *     It is not uncommon for large C software projects to include
 *     assembly language files that are linked to the overall project.
 *     It is difficult to make information about symbols, like:
 *
 *       o C structure field offsets & sizes
 *       o enumeration values
 *       o CPP symbols containing compile-time expressions.
 *
 *     Using linkersets, all of the information can easily be
 *     converted into a format that is amenable to use in an assembly
 *     file.
 *
 *  2. Check symbol attributes between disjoint compilation models.
 *
 *     When building large disparate systems, such as an operating
 *     system and userlevel programs, they will sometimes share data
 *     structures, but have vastly different build configurations.
 *     These different build configurations can have an affect on
 *     alignment, size or presence of symbols.
 *
 *     This system can use the same linkerset as described in #1 to
 *     also check symbol equivalence between differently compiled
 *     source files, allowing teams to have confidence that there are
 *     not issues with symbol geometry.
 *
 * Importantly, when the compiler options
 *
 *   -ffunction-sections
 *   -fdata-sections
 *
 * and the linker option
 *
 *    --gc-sections
 *
 * are used, the linkersets will not be present in the executable
 * produced, so there is no disk, memory or runtime overhead incurred.
 */
#if !defined(SYMBOL_INFORMATION_H_)
#define SYMBOL_INFORMATION_H_

#include <stddef.h>
#include "linkerset.h"

/* SYMINTF_SET_MODULE
 *
 *  If desired, this macro can be used after including this header to
 *  help differentiate the context in which mismatched symbols were
 *  compiled.
 *
 *  The argument should be a string, such as the value of __FILE__.
 *  If it is not used, there will be no way to differentiate between
 *  conflicting symbol definitions.
 *
 *  See the examples/symbol_info/compatability_checker example for a
 *  demonstration of how it works.
 */
static const char *symintf_module_;
#define SYMINTF_SET_MODULE(module_) \
    static const char *symintf_module_ = module_


#define SYMINTF_SET_NAME symintf_desc
#define SYMINTF_SET_TYPE_NAME XCONCAT_(SYMINTF_SET_NAME,_t)

/* symintf_kind_t
 *
 *  Internal type.  Used to specify what type of data is contained in
 *  a linkerset member.
 */
typedef enum symintf_kind_t {
    AK_STRUCT_FIELD,
    AK_SYMBOL_SIZE,
    AK_ENUM_ELEMENT,
    AK_CPP_INTEGER
} symintf_kind_t;


/* struct_field_desc_t
 *
 *  Information stored for a structure field.
 *
 *  The pointer fields may not be NULL.
 */
typedef struct struct_field_desc_t {
    const char          *tname;  /* struct type name. */
    const char          *fname;  /* field name.       */
    size_t               size;   /* sizeof(tname.fname). */
    size_t               offset; /* offsetof(tname, fname) */
} struct_field_desc_t;


/* symbol_size_desc_t
 *
 *  Information stored for a symbol size.
 *
 *  The pointer fields may not be NULL.
 */
typedef struct symbol_size_desc_t {
    const char          *tname; /* struct type name. */
    size_t               size;  /* sizeof(tname). */
} symbol_size_desc_t;


/* enum_desc_t
 *
 *  Information stored for an enum member.
 *
 *  The pointer fields may not be NULL.
 */
typedef struct enum_desc_t {
    const char          *tname; /* enum type name. */
    const char          *ename; /* enum field name. */
    size_t               value; /* value of enum member. */
    size_t               size;  /* sizeof(tname). */
} enum_desc_t;


/* cpp_int_desc_t
 *
 *  Information stored for an preprocessor integer symbol.
 *
 *  The pointer fields may not be NULL.
 */
typedef struct cpp_int_desc_t {
    const char          *sname; /* CPP symbol name. */
    unsigned long        value; /* value of CPP symbol. */
    size_t               size;  /* size of CPP symbol. */
} cpp_int_desc_t;


/* symintf_desc_t
 *
 *  Linkerset element that contains information stored for a C symbol.
 *
 *  module:
 *
 *   inv: module == NULL ==> SYMINTF_SET_MODULE not used.
 *   inv: module != NULL ==> *module is an ASCIIZ string.
 *
 *  file:
 *
 *   The value of __FILE__ where the linkerset element was added.
 *
 *   inv: file != NULL.
 *   inv: file is an ASCIIZ string.
 *
 *  line:
 *
 *   The value of __LINE__ where the linkerset element was added.
 *
 *  key:
 *
 *   An compile-time internally-generated string that is used to sort
 *   the linkerset members into a binary tree when processing.
 *
 *   inv: key != NULL
 *   inv: key is an ASCIIZ string.
 *
 *  l, r:
 *
 *    Internally used pointers that allow all the symbols to be put
 *    into a binary tree.
 *
 *  kind:
 *
 *   This field differentiates the type of data stored in the 'u'
 *   union.
 *
 *  u:
 *
 *   kind == AK_STRUCT_FIELD
 *
 *    u contains a struct_field_desc_t.
 *
 *   kind == AK_SYMBOL_SIZE
 *
 *    u contains a symbol_size_desc_t.
 *
 *   kind == AK_ENUM_ELEMENT
 *
 *    u contains a enum_desc_t.
 *
 *   kind == AK_CPP_INTEGER
 *
 *    u contains a cpp_int_desc_t.
 */
typedef struct SYMINTF_SET_TYPE_NAME {
    const char            **module;
    const char            *file; /* Source file. */
    unsigned               line; /* Line number. */
    const char            *key;
    struct symintf_desc_t *l;
    struct symintf_desc_t *r;
    symintf_kind_t         kind;
    union {
        struct_field_desc_t struct_field;
        symbol_size_desc_t  symbol_size;
        enum_desc_t         enum_member;
        cpp_int_desc_t      cpp_integer;
    } u;
} symintf_desc_t;


/* STRUCT_FIELD_VAR_NAME
 * SYMBOL_SIZE_VAR_NAME
 * ENUM_VAR_NAME
 * CPP_INT_VAR_NAME
 *
 *  These internal are constructed to assign unique variable names to
 *  each member of the linker set in a single source file.
 *
 *   tname_: The 'typedef' name of the structure or enumeration type.
 *
 *   fname_: The name of a field in the structure 'tname_'.
 *
 *   ename_: The name of an enumeration value in 'tname_'.
 *
 *   sname_: The name of an C preprocessor symbol that contains an
 *           integer value.
 */
#define STRUCT_FIELD_VAR_NAME(tname_, fname_)           \
    XCONCAT_(XCONCAT_(struct_field_,                    \
                      XCONCAT_(XCONCAT_(tname_, _),     \
                               XCONCAT_(fname_, __))),  \
             XCONCAT_(_,__LINE__))


#define SYMBOL_SIZE_VAR_NAME(tname_)            \
    XCONCAT_(XCONCAT_(symbol_size_,             \
                      XCONCAT_(tname_, __)),    \
             XCONCAT_(_,__LINE__))


#define ENUM_VAR_NAME(tname_, ename_)                   \
    XCONCAT_(XCONCAT_(enum_,                            \
                      XCONCAT_(XCONCAT_(tname_, _),     \
                               XCONCAT_(ename_, __))),  \
             XCONCAT_(_,__LINE__))

#define CPP_INT_VAR_NAME(sname_)                \
    XCONCAT_(XCONCAT_(cpp_int_,                 \
                      XCONCAT_(sname_, __)),    \
             XCONCAT_(_,__LINE__))

/* SYMINTF_STRUCT_FIELD_ADD
 *
 *
 *
 *
 */
#define SYMINTF_STRUCT_FIELD_ADD(tname_, fname_)                        \
    static symintf_desc_t STRUCT_FIELD_VAR_NAME(tname_, fname_) = {     \
        .module = &symintf_module_,                                     \
        .file   = __FILE__,                                             \
        .key    = XSTRING_(XCONCAT_(XCONCAT_(tname_, _), fname_)),      \
        .line   = __LINE__,                                             \
        .l      = NULL,                                                 \
        .r      = NULL,                                                 \
        .kind   = AK_STRUCT_FIELD,                                      \
        .u.struct_field.tname  = XSTRING_(tname_),                      \
        .u.struct_field.fname  = XSTRING_(fname_),                      \
        .u.struct_field.size   = sizeof(((tname_ *)(0))->fname_),       \
        .u.struct_field.offset = offsetof(tname_, fname_)               \
    };                                                                  \
    LINKERSET_ADD_ITEM(SYMINTF_SET_NAME,                                \
                       STRUCT_FIELD_VAR_NAME(tname_, fname_))


/* SYMINTF_SYMBOL_SIZE_ADD
 *
 *  This adds the size of a symbol to the linkerset.
 *
 *  tname_ must be a symbol to which the C operator sizeof() can be
 *  applied.
 */
#define SYMINTF_SYMBOL_SIZE_ADD(tname_)                         \
    static symintf_desc_t SYMBOL_SIZE_VAR_NAME(tname_) = {      \
        .module = &symintf_module_,                             \
        .file   = __FILE__,                                     \
        .key    = XSTRING_(tname_),                             \
        .line   = __LINE__,                                     \
        .l      = NULL,                                         \
        .r      = NULL,                                         \
        .kind   = AK_SYMBOL_SIZE,                               \
        .u.symbol_size.tname = XSTRING_(tname_),                \
        .u.symbol_size.size  = sizeof(tname_)                   \
    };                                                          \
    LINKERSET_ADD_ITEM(SYMINTF_SET_NAME,                        \
                       SYMBOL_SIZE_VAR_NAME(tname_))


/* SYMINTF_ENUM_ADD
 *
 *  This adds the size of a typedef-ed enumeration element to the
 *  linkerset.  tname_ must be the name of the enumeration type, and
 *  ename_ must be the name of a member of that enumeration.
 *
 *  Enumerations in C do not need to have a corresponding typedef
 *  name, but it is required here to ensure that all enumeration
 *  symbols are uniquely identified in the linkerset.
 */
#define SYMINTF_ENUM_ADD(tname_, ename_)                        \
    static symintf_desc_t ENUM_VAR_NAME(tname_, ename_) = {     \
        .module = &symintf_module_,                             \
        .file   = __FILE__,                                     \
        .key    = XSTRING_(XCONCAT_(XCONCAT_(tname_, _),        \
                                    ename_)),                   \
        .line   = __LINE__,                                     \
        .l      = NULL,                                         \
        .r      = NULL,                                         \
        .kind   = AK_ENUM_ELEMENT,                              \
        .u.enum_member.tname = XSTRING_(tname_),                \
        .u.enum_member.ename = XSTRING_(ename_),                \
        .u.enum_member.value = ename_,                          \
        .u.enum_member.size  = sizeof(tname_)                   \
    };                                                          \
    LINKERSET_ADD_ITEM(SYMINTF_SET_NAME,                        \
                       ENUM_VAR_NAME(tname_, ename_))


/* SYMINTF_CPP_INT_ADD
 *
 *  This adds the value of a CPP symbol to the linkerset.  The symbol
 *  must evaluate to an integer value.
 *
 *  sname_ must be the name of the preprocessor symbol.
 */
#define SYMINTF_CPP_INT_ADD(sname_)                     \
    static symintf_desc_t CPP_INT_VAR_NAME(sname_) = {  \
        .module = &symintf_module_,                     \
        .file   = __FILE__,                             \
        .key    = #sname_,                              \
        .line   = __LINE__,                             \
        .l      = NULL,                                 \
        .r      = NULL,                                 \
        .kind   = AK_CPP_INTEGER,                       \
        .u.cpp_integer.sname = #sname_,                 \
        .u.cpp_integer.value = sname_,                  \
        .u.cpp_integer.size  = sizeof(sname_)           \
    };                                                  \
    LINKERSET_ADD_ITEM(SYMINTF_SET_NAME,                \
                       CPP_INT_VAR_NAME(sname_))


/* Declare all linkersets for module 'mname_'.  The linkersets enable
 * converting C information to a format suitable for use with the Gnu
 * assembler, and include the following:
 *
 *   C symbol size.
 *   C structure fields, offsets and sizes.
 *   Enumeration values.
 *   Preprocessor integer constants.
 *
 * It is also possible to check that disparate modules in a large
 * software system can be linked in to a single executable, and the
 * linkerset can be used to check that shared types & symbols are
 * equivalent in both build contexts.
 */
#define DECLARE_SYMINTF_LINKERSETS              \
    LINKERSET_DECLARE(SYMINTF_SET_NAME);
#endif
