CC = gcc
C_FLAGS = -pg -pedantic

GCC_FLAGS = -Wall -Wextra -fopenmp -fopenacc -fopt-info-all
GCC_DEBUG_FLAGS = -O0 -g
GCC_AUTOVEC_FLAGS = -O2

CLANG_FLAGS = -Weverything -fopenmp -Rpass=.* -Rpass-missed=.*
CLANG_DEBUG_FLAGS = -O0 -g
CLANG_AUTOVEC_FLAGS = -O2

SRCDIR = src
BINDIR = bin
ASMDIR = asm
LOGDIR = log
SRC = $(SRCDIR)/stencil3d.c
ASM = $(ASMDIR)/dummy.s
EXEC = $(BINDIR)/dummy
LOGFILE = $(LOGDIR)/dummy.comp

.PHONY: all clean gcc_debug gcc_autovec clang_debug clang_autovec

all:
	make gcc_debug
	make gcc_autovec
	make clang_debug
	make clang_autovec

gcc_debug: CC = gcc
gcc_debug: C_FLAGS += $(GCC_FLAGS) $(GCC_DEBUG_FLAGS)
gcc_debug: ASM = $(ASMDIR)/gcc_debug.s
gcc_debug: EXEC = $(BINDIR)/gcc_debug
gcc_debug: LOGFILE = $(LOGDIR)/gcc_debug.comp
gcc_debug: $(EXEC)

gcc_autovec: CC = gcc
gcc_autovec: C_FLAGS += $(GCC_FLAGS) $(GCC_AUTOVEC_FLAGS)
gcc_autovec: ASM = $(ASMDIR)/gcc_autovec.s
gcc_autovec: EXEC = $(BINDIR)/gcc_autovec
gcc_autovec: LOGFILE = $(LOGDIR)/gcc_autovec.comp
gcc_autovec: $(EXEC)

clang_debug: CC = clang
clang_debug: C_FLAGS += $(CLANG_FLAGS) $(CLANG_DEBUG_FLAGS)
clang_debug: ASM = $(ASMDIR)/clang_debug.s
clang_debug: EXEC = $(BINDIR)/clang_debug
clang_debug: LOGFILE = $(LOGDIR)/clang_debug.comp
clang_debug: $(EXEC)

clang_autovec: CC = clang
clang_autovec: C_FLAGS += $(CLANG_FLAGS) $(CLANG_AUTOVEC_FLAGS)
clang_autovec: ASM = $(ASMDIR)/clang_autovec.s
clang_autovec: EXEC = $(BINDIR)/clang_autovec
clang_autovec: LOGFILE = $(LOGDIR)/clang_autovec.comp
clang_autovec: $(EXEC)

$(EXEC): $(SRC)
	mkdir -p $(BINDIR) $(ASMDIR) $(LOGDIR)
	$(CC) -S $(SRC) $(C_FLAGS) -o $(ASM) 2>&1 | tee $(LOGFILE)
	$(CC) $(ASM) $(C_FLAGS) -o $(EXEC) 2>&1 | tee -a $(LOGFILE)

clean:
	rm -f $(BINDIR)/* $(ASMDIR)/* $(LOGDIR)/* gmon.out
