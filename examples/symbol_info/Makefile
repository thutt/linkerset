# BSD 2-Clause License
#
# Copyright (c) 2025 Logic Magicians Software (Taylor Hutt)
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#
CFLAGS	=					\
	-I../..					\
	$(addprefix -D,$(DEFINE))		\
	-MMD					\
	-ffunction-sections			\
	-fdata-sections				\
	-Wl,--gc-sections

EXECUTABLES	:=				\
	compatability_checker			\
	ittybitty_interface			\
	imported_interface			\
	doubly_interface			\
	imported_empty


all: $(EXECUTABLES)

# This program shows how separately-compiled sources' object files can
# be linked into a single executable that will verify that all the
# symbols in the linkerset are either unique, or identical.
#
# This is important when you have separately built systems in a large
# software system.  Types can change due to conditional compilation,
# or compiler options; consider how structures shared between
# userspace and kernel space can end up being different in a large
# complicated build system.
#
compatability_checker:	symbol_info_manager.o ittybitty.o imported.o doubly.o
	$(CC) $(CFLAGS) -o $@ $^


# These three programs (ittybitty_interface, imported_interface,
# doubly_interface) show how the 'symbol_info_manager' module can be
# used to produce output of information about C symbols that is usable
# by Gnu Assembler.
#
ittybitty_interface: symbol_info_manager.o ittybitty.o
	$(CC) $(CFLAGS) -o $@ $^


imported_interface: symbol_info_manager.o imported.o
	$(CC) $(CFLAGS) -o $@ $^


doubly_interface: symbol_info_manager.o doubly.o
	$(CC) $(CFLAGS) -o $@ $^


# This programs (imported_empty) is built to specifically not
# reference the linkerset.
#
imported_empty: imported.o emptymain.c
	$(CC) $(CFLAGS) -o $@ $^


# This recipe shows that a linked program (imported_empty) that does
# not reference the linkerset will not have it included in the final
# executable.  This is done dumping the 'symintf_desc' section
# contained in 'imported_empty' (which does not reference the
# linkerset) and 'imported_interface' (which does reference the
# linkerset).
#
# The former shows a message that indicates the section is not present
# in the executable, while the latter does show the section header.
#
# The '-' prefix ensures the error returned from objdump is ignored.
#
section_compare: imported_interface imported_empty
	-objdump --section-headers --section=symintf_desc imported_empty;
	objdump --section-headers --section=symintf_desc imported_interface;


example.o:	example.c
ittybitty.o:	ittybitty.c
ittybitty.o:	DEFINE:=ITTYBITTY
imported.o:	imported.c
imported.o:	DEFINE:=IMPORTED_FRUIT
doubly.o:	doubly.c
doubly.o:	DEFINE:=DOUBLY_LINKED
emptymain.o:	emptymain.c

clean:
	rm -rf $(EXECUTABLES) *.o *.d;

-include *.d
