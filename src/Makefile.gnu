# This is a plain Makefile for general Phorward Foundation Toolkit development.
# It compiles libphorward.a quickly and out of the box without autotools.

CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I.

AWK=awk
LIBPHORWARD = libphorward.a
PROTOFILE =	proto.h
LIBHEADER = phorward.h
PATHEXT = PATH="../run:$(PATH)"

REF = ../doc/ref.t2t

SRC = \
	any/any.c \
	any/any.conv.c \
	any/any.print.c \
	any/any.get.c \
	any/any.set.c \
	base/array.c \
	base/ccl.c \
	base/dbg.c \
	base/list.c \
	base/memory.c \
	parse/ast.c \
	parse/bnf.c \
	parse/gram.c \
	parse/ll.c \
	parse/lr.c \
	parse/parse.c \
	parse/prod.c \
	parse/scan.c \
	parse/sym.c \
	regex/dfa.c \
	regex/direct.c \
	regex/lex.c \
	regex/misc.c \
	regex/nfa.c \
	regex/pattern.c \
	regex/regex.c \
	string/convert.c \
	string/string.c \
	string/utf8.c \
	util/system.c \
	vm/prog.c \
	vm/run.c \
	vm/vm.c

HSRC = \
	base/array.h \
	base/ccl.h \
	base/dbg.h \
	base/list.h \
	parse/parse.h \
	any/any.h \
	vm/vm.h \
	regex/regex.h \
	phorward.tpl.h \
	$(PROTOFILE)

OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(LIBPHORWARD)

$(LIBPHORWARD): $(LIBHEADER) $(OBJ)
	$(AR) rv $@ $(OBJ)

clean:
	-rm -f $(OBJ)
	-rm -f $(LIBPHORWARD)

clean_all: clean
	-rm $(LIBHEADER)
	-rm any/any.conv.c
	-rm any/any.get.c
	-rm any/any.print.c
	-rm any/any.set.c

ref: $(SRC)
	echo "" >$(REF)
	echo "" >>$(REF)
	echo "" >>$(REF)
	pdoc -d 2 $(SRC) >>$(REF)


# Prototypes Files
$(PROTOFILE): $(SRC)
	$(PATHEXT) pproto $(SRC) >$@

# Library Header
$(LIBHEADER): $(HSRC)
	$(PATHEXT) filehead phorward.h "Phorward Foundation Toolkit Global Header" \
		>$@
	$(PATHEXT) pinclude phorward.tpl.h >>$@

# pany object functions (generated from definitions in any.h comments)
any/any.get.c: any/any.h any/any.gen.awk
	$(AWK) -f any/any.gen.awk -vwith_get=1 any/any.h >$@

any/any.set.c: any/any.h any/any.gen.awk
	$(AWK) -f any/any.gen.awk -vwith_set=1 any/any.h >$@

any/any.conv.c: any/any.h any/any.gen.awk
	$(AWK) -f any/any.gen.awk -vwith_conv=1 any/any.h >$@

any/any.print.c: any/any.h any/any.gen.awk
	$(AWK) -f any/any.gen.awk -vwith_fprint=1 any/any.h >$@

