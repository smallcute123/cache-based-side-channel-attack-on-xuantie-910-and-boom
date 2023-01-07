###############################################################################
# Makefile for spectre-attacks
###########################
# Makes baremetal executables to run on xuantie 
###############################################################################

# Folders
SRC:=src
INC:=include
LNK:=link
OBJ:=obj
BIN:=bin
DMP:=dump
DEP:=dep

ALL?=spectreV1

QEMU=/home/chengxiaoyu/xuantie/host/csky-qemu/bin
# Commands and flags
CC:=riscv64-unknown-elf-gcc
OBJDUMP:=riscv64-unknown-elf-objdump -S
CFLAGS= -l -std=gnu99 -O0 -g -Wall -I$(INC)
#CFLAGS=-mcmodel=medany -l -std=gnu99 -O0 -g -fno-common -fno-builtin-printf -Wall -I$(INC) -Wno-unused-function -Wno-unused-variable
#LDFLAGS:=-static -nostdlib -nostartfiles -lgcc
LDFLAGS:=-static -lgcc
DEPFLAGS=-MT $@ -MMD -MP -MF $(DEP)/$*.d
RM=rm -rf

# Programs to compile
PROGRAMS= dcacheTest spectreV1 flushCacheTest condBranchMispred cachetest jfflushCacheTest
BINS=$(addprefix $(BIN)/,$(addsuffix .riscv,$(PROGRAMS)))
DUMPS=$(addprefix $(DMP)/,$(addsuffix .dump,$(PROGRAMS)))

.PHONY: all
all: $(BINS) $(DUMPS)

dumps: $(DUMPS)

# Build object files
$(OBJ)/%.o: $(SRC)/%.S
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -D__ASSEMBLY__=1 -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(OBJ)
	@mkdir -p $(DEP)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Build executable
$(BIN)/%.riscv: $(OBJ)/%.o 
	@mkdir -p $(BIN)
	$(CC) $(LDFLAGS) $<  -o $@

# Build dump
$(DMP)/%.dump: $(BIN)/%.riscv
	@mkdir -p $(DMP)
	$(OBJDUMP) -D $< > $@

# Keep the temporary .o files
.PRECIOUS: $(OBJ)/%.o

# Remove all generated files
clean:
	rm -rf $(BIN) $(OBJ) $(DMP) $(DEP)

run:$(BINS) $(DUMPS)
	$(QEMU)/qemu-riscv64 -cpu c910 bin/$(ALL).riscv

# Include dependencies
-include $(addprefix $(DEP)/,$(addsuffix .d,$(PROGRAMS)))
