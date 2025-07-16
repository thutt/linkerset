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
CFLAGS	= -I../.. -MMD

example:	example.o mod_b.o mod_c.o mod_a.o 

example.o:	example.c
mod_a.o:	mod_a.c
mod_b.o:	mod_b.c
mod_c.o:	mod_c.c


cycle_example:	\
	example.o mod_b.o mod_c.o mod_a.o  \
	mod_cycle_0.o mod_cycle_1.o mod_cycle_2.o
	$(CC) $(CFLAGS) -o $@ $^

init_error_example:	\
	example.o mod_b.o mod_c.o mod_a.o  \
	mod_error_init.o
	$(CC) $(CFLAGS) -o $@ $^

fina_error_example:	\
	example.o mod_b.o mod_c.o mod_a.o  \
	mod_error_fina.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf example *.o *.d;

-include *.d
