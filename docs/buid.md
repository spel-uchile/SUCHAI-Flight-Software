Build instructions
=

Build for Atmel AV32UC3 Xplained
==

1. Install dfu programmer

	sudo apt-get install dfu-programer

2. Install ASF and drivers

	python3 compile.py FREERTOS AVR32 --drivers

3. Build (no communications, no permanent storage)

	python3 compile.py FREERTOS AVR32 --sch_com 0 --sch_st_mode 0

4. Program using dfu-programmer

	Reset device while pressing the SW0 button
	cd src/drivers/atmel/
	sudo sh build.sh dfu
	

