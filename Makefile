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

gcc_base: CC = gcc
gcc_base: C_FLAGS += $(GCC_DEBUG_FLAGS)
gcc_base: SRC = $(SRCDIR)/stencil3d.c
gcc_base: EXEC = $(BINDIR)/gcc_base
gcc_base: $(EXEC)

gcc_inv_loop: CC = gcc
gcc_inv_loop: C_FLAGS += $(GCC_DEBUG_FLAGS)
gcc_inv_loop: SRC = $(SRCDIR)/stencil3d_inv_loop.c
gcc_inv_loop: EXEC = $(BINDIR)/gcc_inv_loop
gcc_inv_loop: $(EXEC)

gcc_inv_loop_autovec: CC = gcc
gcc_inv_loop_autovec: C_FLAGS += $(GCC_AUTOVEC_FLAGS)
gcc_inv_loop_autovec: SRC = $(SRCDIR)/stencil3d_inv_loop.c
gcc_inv_loop_autovec: EXEC = $(BINDIR)/gcc_inv_loop_autovec
gcc_inv_loop_autovec: $(EXEC)

clang_base: CC = clang
clang_base: C_FLAGS += $(CLANG_DEBUG_FLAGS)
clang_base: SRC = $(SRCDIR)/stencil3d.c
clang_base: EXEC = $(BINDIR)/clang_base
clang_base: $(EXEC)

clang_inv_loop: CC = clang
clang_inv_loop: C_FLAGS += $(CLANG_DEBUG_FLAGS)
clang_inv_loop: SRC = $(SRCDIR)/stencil3d_inv_loop.c
clang_inv_loop: EXEC = $(BINDIR)/clang_inv_loop
clang_inv_loop: $(EXEC)

clang_inv_loop_autovec: CC = clang
clang_inv_loop_autovec: C_FLAGS += $(CLANG_AUTOVEC_FLAGS)
clang_inv_loop_autovec: SRC = $(SRCDIR)/stencil3d_inv_loop.c
clang_inv_loop_autovec: EXEC = $(BINDIR)/clang_inv_loop_autovec
clang_inv_loop_autovec: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(SRC) $(C_FLAGS) -o $(EXEC)

clean:
	rm -f ./bin/*
