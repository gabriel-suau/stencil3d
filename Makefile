CC = gcc
C_FLAGS = -pg -pedantic

GCC_DEBUG_FLAGS = -O0 -g -Wall
GCC_AUTOVEC_FLAGS = -O2 -Wall -ftree-vectorize -fopt-info-vec-all

CLANG_DEBUG_FLAGS = -O0 -g -Weverything
CLANG_AUTOVEC_FLAGS = -O2 -Weverything -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -Rpass=slp-vectorize -Rpass-missed=slp-vectorize -Rpass-analysis=slp-vectorize

SRCDIR = src
BINDIR = bin
SRC = $(SRCDIR)/stencil3d.c
EXEC = $(BINDIR)/gcc_base

gcc_debug: CC = gcc
gcc_debug: C_FLAGS += $(GCC_DEBUG_FLAGS)
gcc_debug: SRC = $(SRCDIR)/stencil3d.c
gcc_debug: EXEC = $(BINDIR)/gcc_debug
gcc_debug: $(EXEC)

gcc_autovec: CC = gcc
gcc_autovec: C_FLAGS += $(GCC_AUTOVEC_FLAGS)
gcc_autovec: SRC = $(SRCDIR)/stencil3d.c
gcc_autovec: EXEC = $(BINDIR)/gcc_autovec
gcc_autovec: $(EXEC)

clang_debug: CC = clang
clang_debug: C_FLAGS += $(CLANG_DEBUG_FLAGS)
clang_debug: SRC = $(SRCDIR)/stencil3d.c
clang_debug: EXEC = $(BINDIR)/clang_debug
clang_debug: $(EXEC)

clang_autovec: CC = clang
clang_autovec: C_FLAGS += $(CLANG_AUTOVEC_FLAGS)
clang_autovec: SRC = $(SRCDIR)/stencil3d.c
clang_autovec: EXEC = $(BINDIR)/clang_autovec
clang_autovec: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(SRC) $(C_FLAGS) -o $(EXEC)

clean:
	rm -f $(BINDIR)/*
