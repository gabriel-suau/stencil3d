##################################################
# Makefile
#
# Easy compilation of multiple targets with same
# sources but differents compilers and options.
#
# Author : Gabriel Suau
##################################################

CC = gcc
C_FLAGS = -pedantic -fopenmp -march=native

GCC_FLAGS = -Wall -Wextra -fverbose-asm -fopenacc -fopt-info-all
GCC_DEBUG_FLAGS = -O0 -g
GCC_AUTOVEC_FLAGS = -O3

CLANG_FLAGS = -Weverything -Rpass=.* -Rpass-missed=.*
CLANG_DEBUG_FLAGS = -O0 -g
CLANG_AUTOVEC_FLAGS = -O3

ICC_FLAGS = -Wall -Wextra
ICC_DEBUG_FLAGS = -O0 -g
ICC_AUTOVEC_FLAGS = -O3

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

icc_debug: CC = icc
icc_debug: C_FLAGS += $(ICC_FLAGS) $(ICC_DEBUG_FLAGS)
icc_debug: ASM = $(ASMDIR)/icc_debug.s
icc_debug: EXEC = $(BINDIR)/icc_debug
icc_debug: LOGFILE = $(LOGDIR)/icc_debug.comp
icc_debug: $(EXEC)

icc_autovec: CC = icc
icc_autovec: C_FLAGS += $(ICC_FLAGS) $(ICC_AUTOVEC_FLAGS)
icc_autovec: ASM = $(ASMDIR)/icc_autovec.s
icc_autovec: EXEC = $(BINDIR)/icc_autovec
icc_autovec: LOGFILE = $(LOGDIR)/icc_autovec.comp
icc_autovec: $(EXEC)

$(EXEC): $(SRC)
	mkdir -p $(BINDIR) $(ASMDIR) $(LOGDIR)
	$(CC) -S $(SRC) $(C_FLAGS) -o $(ASM) 2>&1 | tee $(LOGFILE)
	$(CC) $(ASM) $(C_FLAGS) -o $(EXEC) 2>&1 | tee -a $(LOGFILE)

clean:
	rm -f $(BINDIR)/* $(ASMDIR)/* $(LOGDIR)/* gmon.out
