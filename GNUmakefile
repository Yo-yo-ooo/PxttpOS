
all: 
	rm System.iso || true
	$(MAKE) System.iso
	true || $(MAKE) cleanError

# for nvim users apparently
CRun:	
	./CRUN.sh

.PHONY: kernel
kernel:
	$(MAKE) -C libm
	$(MAKE) -C kernel
	$(MAKE) -C modules
	$(MAKE) -C programs
	$(MAKE) -C kernel-loader

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v4.x-branch-binary --depth=1
	make -C limine

System.iso:
	# $(MAKE) cleanObjFolder --silent
	$(MAKE) cleanExternalFolder --silent
	$(MAKE) kernel
	rm -rf iso_root
	mkdir -p iso_root
	# cp modules/test/test.elf external/test.elf
	# cp modules/nothing-doer/nothing-doer.elf external/nothing-doer.elf
	
	for i in ./objects/modules/*/; do \
		if [ -d "$$i" ]; \
		then \
			echo "$$(basename "$$i")"; \
			mkdir -p objects/external/modules/$$(basename "$$i")/assets; \
			cp "$$i/$$(basename "$$i").elf" objects/external/modules/$$(basename "$$i")/$$(basename "$$i").elf; \
			\
			if [ -d "./modules/$$(basename "$$i")/assets" ]; then \
				cp -r "./modules/$$(basename "$$i")/assets" objects/external/modules/$$(basename "$$i"); \
			fi; \
		fi \
	done
	
	for i in ./objects/programs/*/; do \
		if [ -d "$$i" ]; \
		then \
			echo "$$(basename "$$i")"; \
			mkdir -p objects/external/programs/$$(basename "$$i")/assets; \
			cp "$$i/$$(basename "$$i").elf" objects/external/programs/$$(basename "$$i")/$$(basename "$$i").elf; \
			\
			if [ -d "./programs/$$(basename "$$i")/assets" ]; then \
				cp -r "./programs/$$(basename "$$i")/assets" objects/external/programs/$$(basename "$$i"); \
			fi; \
		fi \
	done
	

	$(MAKE) -C saf
	./saf/saf-make ./objects/external ./external/programs.saf -q
	
	
	
	cp objects/kernel-loader/kernel.elf \
		limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin \
		external/* \
		iso_root/
		
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o System.iso
		
	limine/limine-deploy System.iso
	rm -rf iso_root


cleanError:
	$(MAKE) clean2 
	$(error "error happened")

clean: clean2
	@rm -rf iso_root System.iso barebones.hdd ./external/programs.saf
	

clean2:
	@rm -rf iso_root barebones.hdd ./external/programs.saf
	@$(MAKE) cleanObjFolder
	@$(MAKE) -C libm clean
	@$(MAKE) -C kernel clean
	@$(MAKE) -C modules clean
	@$(MAKE) -C programs clean
	@$(MAKE) -C kernel-loader clean
	@$(MAKE) -C saf clean


cleanObjFolder:
	@rm -rf objects || true
	@mkdir objects
	@mkdir objects/kernel
	@mkdir objects/kernel-loader
	@mkdir objects/libm
	@mkdir objects/modules
	@mkdir objects/programs
	@$(MAKE) cleanExternalFolder
	
cleanExternalFolder:
	@rm -rf objects/external || true
	@mkdir objects/external
	@mkdir objects/external/modules
	@mkdir objects/external/programs
	
	
ac:
	make clean -j$(nproc) && make clean2 -j$(nproc)
	make -j$(nproc)
	qemu-system-x86_64 -machine q35 -m 2G -cpu qemu64 -smp 4 -serial stdio -boot d -cdrom System.iso -no-reboot --no-shutdown
