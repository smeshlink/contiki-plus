all:
	${MAKE} TARGET=mxusbstick NOAVRSIZE=1 -f Makefile.mxusbstick mxusbstick.elf
	avr-objcopy -O ihex -R .signature -R .fuse -R .eeprom mxusbstick.elf mxusbstick.hex
	avr-size  -C  --mcu=at90usb1287 mxusbstick.elf

clean:
	${MAKE} -f Makefile.mxusbstick clean
	rm -rf obj_mxusbstick
	rm symbols.c symbols.h mxusbstick.elf mxusbstick.hex
