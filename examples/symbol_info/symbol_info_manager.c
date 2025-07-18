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
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "symbol_info.h"

DECLARE_SYMINTF_LINKERSETS;

SYMINTF_SET_TYPE_NAME *linkerset_root;
int program_return_code;

static void
show_compilation_model(SYMINTF_SET_TYPE_NAME *p)
{
    if (*p->module != NULL) {
        fprintf(stderr, "  compiled: %s\n", *p->module);
    } else {
        fprintf(stderr, "  compiled: <SYMINTF_SET_MODULE not used>\n");
    }
}


static void
struct_field_error(const char *hdr, SYMINTF_SET_TYPE_NAME *p)
{
    struct_field_desc_t *sp = &p->u.struct_field;
    fprintf(stderr,
            "%s %s:%u %s.%s: (%zd bytes, offset: %zd)",
            hdr, p->file, p->line,
            sp->tname, sp->fname, sp->size, sp->offset);
    show_compilation_model(p);
 }


static void
symbol_size_error(const char *hdr, SYMINTF_SET_TYPE_NAME *p)
{
    symbol_size_desc_t *sp = &p->u.symbol_size;

    fprintf(stderr,
            "%s %s:%u %s: %zd bytes",
            hdr, p->file, p->line, sp->tname, sp->size);
    show_compilation_model(p);
 }

static void
enum_error(const char *hdr, SYMINTF_SET_TYPE_NAME *p)
{
    enum_desc_t *sp = &p->u.enum_member;

    fprintf(stderr,
            "%s %s:%u %s.%s: value %zd",
            hdr, p->file, p->line,
            sp->tname, sp->ename, sp->value);
    show_compilation_model(p);
 }


static void
cpp_integer_error(const char *hdr, SYMINTF_SET_TYPE_NAME *p)
{
    cpp_int_desc_t *sp = &p->u.cpp_integer;

    fprintf(stderr, "%s %s:%u %s  value %zd",
            hdr, p->file, p->line, sp->sname, sp->value);
    show_compilation_model(p);
 }


static void
compare_structure_field(SYMINTF_SET_TYPE_NAME *lp,
                        SYMINTF_SET_TYPE_NAME *rp)

{
    struct_field_desc_t *l = &lp->u.struct_field;
    struct_field_desc_t *r = &rp->u.struct_field;
    if (l->size != r->size || l->offset != r->offset) {
        program_return_code = 1; /* Error */
        struct_field_error("FIELD", lp);
        struct_field_error("     ", rp);
        fprintf(stderr, "\n");
    }
}


static void
compare_symbol_size(SYMINTF_SET_TYPE_NAME *lp, SYMINTF_SET_TYPE_NAME *rp)
{
    symbol_size_desc_t *l = &lp->u.symbol_size;
    symbol_size_desc_t *r = &rp->u.symbol_size;

    if (l->size != r->size) {
        symbol_size_error("SIZE ", lp);
        symbol_size_error("     ", rp);
        fprintf(stderr, "\n");
    }
}


static void
compare_enum(SYMINTF_SET_TYPE_NAME *lp, SYMINTF_SET_TYPE_NAME *rp)
{
    enum_desc_t *l = &lp->u.enum_member;
    enum_desc_t *r = &rp->u.enum_member;;

    if (l->value != r->value) {
        enum_error("ENUM ", lp);
        enum_error("     ", rp);
        fprintf(stderr, "\n");
    }
}


static void
compare_cpp_integer(SYMINTF_SET_TYPE_NAME *lp, SYMINTF_SET_TYPE_NAME *rp)
{
    cpp_int_desc_t *l = &lp->u.cpp_integer;
    cpp_int_desc_t *r = &rp->u.cpp_integer;

    if (l->value != r->value) {
        cpp_integer_error("CPP  ", lp);
        cpp_integer_error("     ", rp);
        fprintf(stderr, "\n");
    }
}


static void
add_set_element(SYMINTF_SET_TYPE_NAME **root, SYMINTF_SET_TYPE_NAME *elem)
{
    if (*root == NULL) {
        *root = elem;           /* Empty tree. */
    } else {
        int cmp = strcmp((*root)->key, elem->key);
        if (cmp > 0) {
            /* Root > elem->key */
            add_set_element(&(*root)->l, elem);
        } else if (cmp < 0) {
            add_set_element(&(*root)->r, elem);
        } else {
            /* Because the value of the key is defined by this system,
             * when 'key' matches, the 'kind' must also match.
             */
            assert(elem->kind == (*root)->kind);

            switch (elem->kind) {

            case AK_STRUCT_FIELD: {
                compare_structure_field(*root, elem);
                break;
            }

            case AK_SYMBOL_SIZE: {
                compare_symbol_size(*root, elem);
                break;
            }

            case AK_ENUM_ELEMENT: {
                compare_enum(*root, elem);
                break;
            }

            case AK_CPP_INTEGER: {
                compare_cpp_integer(*root, elem);
                break;
            }
            }
        }
    }
}


static void
print_tree_struct_field(SYMINTF_SET_TYPE_NAME *elem)
{
    struct_field_desc_t *p = &elem->u.struct_field;
    printf("#define OFFSET__%s_%s (%zd)\n", p->tname, p->fname, p->offset);
    printf("#define SIZE__%s_%s (%zd)\n", p->tname, p->fname, p->size);
}


static void
print_tree_symbol_size(SYMINTF_SET_TYPE_NAME *elem)
{
    symbol_size_desc_t *p = &elem->u.symbol_size;
    printf("#define SIZE__%s (%zd)\n", p->tname, p->size);
}


static void
print_tree_enum_element(SYMINTF_SET_TYPE_NAME *elem)
{
    enum_desc_t *p = &elem->u.enum_member;
    printf("#define %s_%s (%zd)\n", p->tname, p->ename, p->value);
}


static void
print_tree_cpp_integer(SYMINTF_SET_TYPE_NAME *elem)
{
    cpp_int_desc_t *p = &elem->u.cpp_integer;
    printf("#define %s (%zd)\n", p->sname, p->value);

}


static void
print_tree(SYMINTF_SET_TYPE_NAME *elem)
{
    if (elem != NULL) {
        print_tree(elem->l);
        switch (elem->kind) {
        case AK_STRUCT_FIELD: {
            print_tree_struct_field(elem);
            break;
        }

        case AK_SYMBOL_SIZE: {
            print_tree_symbol_size(elem);
            break;
        }

        case AK_ENUM_ELEMENT: {
            print_tree_enum_element(elem);
            break;
        }

        case AK_CPP_INTEGER: {
            print_tree_cpp_integer(elem);
            break;
        }

        default: {
            fprintf(stderr, "internal error: invalid element kind: %d\n",
                    elem->kind);
            exit(100);
            break;
        }
        }
        print_tree(elem->r);
    }
}


int
main(void)
{
    linkerset_root      = NULL;
    program_return_code = 0;

    LINKERSET_ITERATE(SYMINTF_SET_NAME, p, {
        SYMINTF_SET_TYPE_NAME *tp = p;

        add_set_element(&linkerset_root, tp);
    });

    if (program_return_code == 0) {
        print_tree(linkerset_root);
    }
    return program_return_code;
}
