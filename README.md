

# Nanosatellite Flight Software
## Key Features
* Designed for medium-range microcontrollers, as Microchip PIC24.
* Low memory foot print
* Highly extensible command processor architecture
* Powered by FreeRTOS

Visit http://suchai.li2.uchile.cl/?page_id=52 to get latest news about SUCHAI project
Visit http://www.freertos.org/ to get FreeRTOS source code and documentation
## Installation Notes
Suchai software is disagned to run in multiple architectures trough RTOS, also is ported to Linux for easy debugging. 
Currently have been probed in listed OS/Architectures :

* Arch Linux 64-bit
* Manjaro 64-bit
### Linux Installation
#### Library Requirements
Linux installation requires listed libraries
* lpthread 
* lsqlite3
#### Commands  and Run
clone this repository
```bash
git clone https://github.com/spel-uchile/SUCHAI-Flight-Software
```
go to folder, build with cmake and run software
```bash
cd SUCHAI-Flight-Software
mkdir build
cd build
cmake ..
make
./SUCHAI_Flight_Software



```
