# Nuke built-in rules and variables.
override MAKEFLAGS += -rR

# This is the name that our final kernel executable will have.
# Change as needed.
override KERNEL := kernel

# Convenience macro to reliably declare user overridable variables.
define DEFAULT_VAR =
    ifeq ($(origin $1),default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1),undefined)
        override $(1) := $(2)
    endif
endef

# It is suggested to use a custom built cross toolchain to build a kernel.
# We are using the standard "c++" here, it may work by using
# the host system's toolchain, but this is not guaranteed.
override DEFAULT_CXX := c++
$(eval $(call DEFAULT_VAR,CXX,$(DEFAULT_CXX)))

# Same thing for "cc" (the C compiler).
override DEFAULT_CC := cc
$(eval $(call DEFAULT_VAR,CC,$(DEFAULT_CC)))

# Same thing for "ld" (the linker).
override DEFAULT_LD := ld
$(eval $(call DEFAULT_VAR,LD,$(DEFAULT_LD)))

# User controllable C++ flags.
override DEFAULT_CXXFLAGS := -g -O0 -pipe -Wall -Wextra -Wpointer-arith -Wcast-align -Wswitch-enum -DDEBUG_KASSERTS -Isrc -Iext

$(eval $(call DEFAULT_VAR,CXXFLAGS,$(DEFAULT_CXXFLAGS)))

# User controllable C flags.
override DEFAULT_CFLAGS := -g -O0 -pipe
#override DEFAULT_CFLAGS := -g 
$(eval $(call DEFAULT_VAR,CFLAGS,$(DEFAULT_CFLAGS)))

# User controllable C/C++ preprocessor flags. We set none by default.
override DEFAULT_CPPFLAGS :=
$(eval $(call DEFAULT_VAR,CPPFLAGS,$(DEFAULT_CPPFLAGS)))

# User controllable nasm flags.
override DEFAULT_NASMFLAGS := -F dwarf -g
$(eval $(call DEFAULT_VAR,NASMFLAGS,$(DEFAULT_NASMFLAGS)))

# User controllable linker flags. We set none by default.
override DEFAULT_LDFLAGS := #-Map=output.map 
$(eval $(call DEFAULT_VAR,LDFLAGS,$(DEFAULT_LDFLAGS)))

# Internal flags shared by both C and C++ compilers.
override SHARED_FLAGS := \
		-g \
		-O0 \
    -Wall \
    -Wextra \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
		-nostdlib \
    -fno-lto \
    -fPIE \
		-fPIC \
    -m64 \
    -march=x86-64 \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone\
		-isystem include

# Internal C++ flags that should not be changed by the user.
override CXXFLAGS += \
    -std=gnu++20 \
    -fno-rtti \
    -fno-exceptions \
    $(SHARED_FLAGS)

# Internal C flags that should not be changed by the user.
override CFLAGS += \
    -std=gnu11 \
    $(SHARED_FLAGS)

# Internal C/C++ preprocessor flags that should not be changed by the user.
override CPPFLAGS := \
    $(CPPFLAGS) \
    -MMD \
    -MP

# Internal linker flags that should not be changed by the user.
override LDFLAGS += \
    -m elf_x86_64 \
    -nostdlib \
    -static \
    --no-dynamic-linker \
    -z text \
    -z max-page-size=0x1000 \
    -T scripts/linker.ld

#override LDFLAGS += \
    #-m elf_x86_64 \
    #-nostdlib \
    #-static \
    #--no-dynamic-linker \
    #-z text \
    #-z max-page-size=0x1000 \
    #-T linker.ld

# Internal nasm flags that should not be changed by the user.
override NASMFLAGS += \
    -Wall \
    -f elf64

# Use "find" to glob all *.cpp, *.c, *.S, and *.asm files in the tree and obtain the
# object and header dependency file names.
override CXXFILES := $(shell cd src && find -L * -type f -name '*.cpp')
override CFILES := $(shell cd src && find -L * -type f -name '*.c')
override ASFILES := $(shell cd src && find -L * -type f -name '*.S')
override NASMFILES := $(shell cd src && find -L * -type f -name '*.asm')
override OBJ := $(addprefix build/obj/,$(CXXFILES:.cpp=.cpp.o) $(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
override HEADER_DEPS := $(addprefix build/obj/,$(CXXFILES:.cpp=.cpp.d) $(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

# Default target.
.PHONY: all
all: build/bin/$(KERNEL)

src/firmware/limine/limine.h:
	curl -Lo $@ https://github.com/limine-bootloader/limine/raw/trunk/limine.h

# Link rules for the final kernel executable.
build/bin/$(KERNEL): GNUmakefile scripts/linker.ld $(OBJ)
	mkdir -p "$$(dirname $@)"
	$(LD) $(OBJ) $(LDFLAGS) -o $@

# Include header dependencies.
-include $(HEADER_DEPS)

# Compilation rules for *.cpp files.
build/obj/%.cpp.o: src/%.cpp GNUmakefile src/firmware/limine/limine.h
	mkdir -p "$$(dirname $@)"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Compilation rules for *.c files.
build/obj/%.c.o: src/%.c GNUmakefile src/firmware/limine/limine.h
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compilation rules for *.S files.
build/obj/%.S.o: src/%.S GNUmakefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compilation rules for *.asm (nasm) files.
build/obj/%.asm.o: src/%.asm GNUmakefile
	mkdir -p "$$(dirname $@)"
	nasm $(NASMFLAGS) $< -o $@

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf build

.PHONY: distclean
distclean: clean
	rm -f src/firmware/limine/limine.h
