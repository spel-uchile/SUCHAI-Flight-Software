## ATMEL AVR32 Drivers and Framework    

### Installation
To download the AVR32 compilers and ASF (divers and framework), and to set
the development environment run:

    sh install.sh

You will se the folder `suchai` inside `xdk-asf-3.33.0/avr32/applications/`.
This directory contains source code specific to the AVR32 and symlink to the
SUCHAI Flight Sofware source code.

    NOTE: If you need to update code inside src/drivers/atmel/xdk-asf-3.33.0/avr32/applications/suchai,
    please do the same to src/driver/atmel/suchai and commit changes.
    
Install the dfu-programmer if required

* Ubuntu: `sudo apt-get install dfu-programmer`
* Arch:   `yaourt -S dfu-programmer`

### Build and program
To build and program, connect the AVR32 to your computer (using the USB cable)
and then execute

    sh build.sh
    sh build.sh dfu
