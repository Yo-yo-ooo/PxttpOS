include ../defaults.mk
OUTPUT = kernel.elf
OBJDIR = ../objects/kernel-loader

LDFLAGS = $(LD_KERNEL_LOADER_FLAGS)
CPPFLAGS = $(CPP_RELO_FLAGS)

CPPFILES = $(shell find . -type f -name '*.cpp')
CFILES   = $(shell find . -type f -name '*.c')

OBJ      =  $(patsubst %.cpp, $(OBJDIR)/%.o, $(CPPFILES))
OBJ      += $(patsubst %.c, $(OBJDIR)/%.o, $(CFILES))

.PHONY: all
all: $(OUTPUT)

$(OUTPUT): $(OBJ) ../objects/kernel/FINAL_kernel.o ../objects/libm/FINAL_libm.o.kernel_src
	$(LD) $(OBJ) ../objects/kernel/FINAL_kernel.o ../objects/libm/FINAL_libm.o.kernel_src $(LDFLAGS) -o $(OBJDIR)/$@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $^ -o $@ -D _KERNEL_SRC

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(C_RELO_FLAGS) -c $^ -o $@ -D _KERNEL_SRC

flanterm:
	git clone https://github.com/mintsuki/flanterm
	$(MAKE) -C flanterm

.PHONY: clean
clean:
	@rm -rf $(OUTPUT) $(OBJ)
