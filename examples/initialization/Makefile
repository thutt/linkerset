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

EXECUTABLES	:=				\
	example cycle_example			\
	init_error_example			\
	fina_error_example			\
	print_order_example


all:	$(EXECUTABLES)


example:	example.o mod_b.o mod_c.o mod_a.o

example.o:	example.c
mod_a.o:	mod_a.c
mod_b.o:	mod_b.c
mod_c.o:	mod_c.c
mod_d.o:	mod_d.c
mod_e.o:	mod_e.c
mod_f.o:	mod_f.c
mod_g.o:	mod_g.c
mod_h.o:	mod_h.c
mod_i.o:	mod_i.c
mod_j.o:	mod_j.c
mod_k.o:	mod_k.c
mod_l.o:	mod_l.c
mod_m.o:	mod_m.c
mod_n.o:	mod_n.c
mod_o.o:	mod_o.c
mod_p.o:	mod_p.c
mod_q.o:	mod_q.c
mod_r.o:	mod_r.c
mod_s.o:	mod_s.c
mod_t.o:	mod_t.c
mod_u.o:	mod_u.c
mod_v.o:	mod_v.c
mod_w.o:	mod_w.c
mod_x.o:	mod_x.c
mod_y.o:	mod_y.c
mod_z.o:	mod_z.c

print_order_example:				\
	print_order.o				\
	mod_a.o					\
	mod_b.o					\
	mod_c.o					\
	mod_d.o					\
	mod_e.o					\
	mod_f.o					\
	mod_g.o					\
	mod_h.o					\
	mod_i.o					\
	mod_j.o					\
	mod_k.o					\
	mod_l.o					\
	mod_m.o					\
	mod_n.o					\
	mod_o.o					\
	mod_p.o					\
	mod_q.o					\
	mod_r.o					\
	mod_s.o					\
	mod_t.o					\
	mod_u.o					\
	mod_v.o					\
	mod_w.o					\
	mod_x.o					\
	mod_y.o					\
	mod_z.o

	$(CC) $(CFLAGS) -o $@ $^


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
	rm -rf $(EXECUTABLES) *.o *.d;

-include *.d
