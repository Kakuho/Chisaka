# Nuke built-in rules and variables.
override MAKEFLAGS += -rR

override IMAGE_NAME := chisaka

# Convenience macro to reliably declare user overridable variables.
define DEFAULT_VAR =
    ifeq ($(origin $1),default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1),undefined)
        override $(1) := $(2)
    endif
endef

# Toolchain for building the 'limine' executable for the host.
override DEFAULT_HOST_CC := cc
$(eval $(call DEFAULT_VAR,HOST_CC,$(DEFAULT_HOST_CC)))
override DEFAULT_HOST_CFLAGS := -g -O2 -pipe
$(eval $(call DEFAULT_VAR,HOST_CFLAGS,$(DEFAULT_HOST_CFLAGS)))
override DEFAULT_HOST_CPPFLAGS :=
$(eval $(call DEFAULT_VAR,HOST_CPPFLAGS,$(DEFAULT_HOST_CPPFLAGS)))
override DEFAULT_HOST_LDFLAGS :=
$(eval $(call DEFAULT_VAR,HOST_LDFLAGS,$(DEFAULT_HOST_LDFLAGS)))
override DEFAULT_HOST_LIBS :=
$(eval $(call DEFAULT_VAR,HOST_LIBS,$(DEFAULT_HOST_LIBS)))

.PHONY: all
all: $(IMAGE_NAME).iso

.PHONY: all-hdd
all-hdd: $(IMAGE_NAME).hdd

# compile and run the image

.PHONY: run
run: $(IMAGE_NAME).iso
	qemu-system-x86_64 -cpu IvyBridge -M q35 -m 8G -no-reboot \
										 -cdrom $(IMAGE_NAME).iso -hda disk.img \
										 -boot d 

#	compile and run qemu, treating host terminal as serial out

.PHONY: run-io
run-io: $(IMAGE_NAME).iso
	clear &&\
	qemu-system-x86_64 -cpu IvyBridge -M q35 -m 8G  \
										 -serial stdio \
										 -cdrom $(IMAGE_NAME).iso -hda disk.img \
										 -boot d

#	compile and run qemu, treating host terminal as output for the qemu monitor

.PHONY: run-io-monitor
run-io-monitor: $(IMAGE_NAME).iso
	qemu-system-x86_64 -cpu IvyBridge -M q35 -m 8G \
										 -monitor stdio \
										 -cdrom $(IMAGE_NAME).iso -hda disk.img \
										 -boot d

#	compile and run qemu, and print out interrupts to host terminal

.PHONY: run-int
run-int: $(IMAGE_NAME).iso
	clear &&\
	qemu-system-x86_64 -cpu IvyBridge -M q35 -m 8G -no-reboot \
										 -d int -M smm=off \
									   -cdrom $(IMAGE_NAME).iso -hda disk.img \
										 -boot d 

#	test routine for disk storage only

.PHONY: run-disk
run-disk: $(IMAGE_NAME).iso
	qemu-system-x86_64 \
		-cpu IvyBridge -M q35 -m 8G -no-reboot \
		-drive if=ide,file=$(IMAGE_NAME).iso,media=disk,format=raw,bus=2,unit=0 \
		-drive if=ide,file=disk.img,media=disk,format=raw,bus=3,unit=0 \
		-boot d 

# compile and run under gdb debugger

.PHONY: run-gdb
run-gdb: $(IMAGE_NAME).iso
	qemu-system-x86_64 \
		-cpu IvyBridge -M q35 -m 8G -no-reboot\
		-s -S \
		-cdrom $(IMAGE_NAME).iso -hda disk.img \
		-boot d & \
	gdb -ex "target remote localhost:1234" -ex "symbol-file ./kernel/build/bin/kernel"

.PHONY: run-uefi
run-uefi: ovmf $(IMAGE_NAME).iso
	qemu-system-x86_64 -M q35 -m 2G -bios ovmf/OVMF.fd -cdrom $(IMAGE_NAME).iso -boot d

.PHONY: run-hdd
run-hdd: $(IMAGE_NAME).hdd
	qemu-system-x86_64 -nodefaults 	\
										 -display gtk \
										 -vga cirrus\
										 -chardev vc,id=serial\
										 -chardev vc,id=monitor\
										 -serial chardev:serial\
										 -mon chardev=monitor\
										 -M pc-q35-6.1 -m 8G \
										 -drive if=ide,file=$(IMAGE_NAME).hdd,format=raw,media=disk,bus=0,unit=0 \

.PHONY: run-hdd-uefi
run-hdd-uefi: ovmf $(IMAGE_NAME).hdd
	qemu-system-x86_64 -M q35 -m 2G -bios ovmf/OVMF.fd -hda $(IMAGE_NAME).hdd

ovmf:
	mkdir -p ovmf
	cd ovmf && curl -Lo OVMF.fd https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v7.x-binary --depth=1
	$(MAKE) -C limine \
		CC="$(HOST_CC)" \
		CFLAGS="$(HOST_CFLAGS)" \
		CPPFLAGS="$(HOST_CPPFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		LIBS="$(HOST_LIBS)"

.PHONY: kernel
kernel:
	$(MAKE) -C kernel

$(IMAGE_NAME).iso: limine kernel
	rm -rf iso_root
	mkdir -p iso_root/boot
	cp -v kernel/build/bin/kernel iso_root/boot/
	mkdir -p iso_root/boot/limine
	cp -v limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/boot/limine/
	mkdir -p iso_root/EFI/BOOT
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(IMAGE_NAME).iso
	./limine/limine bios-install $(IMAGE_NAME).iso
	rm -rf iso_root

$(IMAGE_NAME).hdd: limine kernel
	rm -f $(IMAGE_NAME).hdd
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(IMAGE_NAME).hdd
	sgdisk $(IMAGE_NAME).hdd -n 1:2048 -t 1:ef00
	./limine/limine bios-install $(IMAGE_NAME).hdd
	mformat -i $(IMAGE_NAME).hdd@@1M
	mmd -i $(IMAGE_NAME).hdd@@1M ::/EFI ::/EFI/BOOT ::/boot ::/boot/limine
	mcopy -i $(IMAGE_NAME).hdd@@1M kernel/build/bin/kernel ::/boot
	mcopy -i $(IMAGE_NAME).hdd@@1M limine.cfg limine/limine-bios.sys ::/boot/limine
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/BOOTIA32.EFI ::/EFI/BOOT

.PHONY: clean
clean:
	rm -rf iso_root $(IMAGE_NAME).iso $(IMAGE_NAME).hdd
	$(MAKE) -C kernel clean

.PHONY: distclean
distclean: clean
	rm -rf limine ovmf
	$(MAKE) -C kernel distclean
