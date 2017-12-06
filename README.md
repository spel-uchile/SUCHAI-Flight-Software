# Nanosatellite Flight Software

The SUCHAI Flight software was originally developed to be used in the 
[SUCHAI](http://spel.ing.uchile.cl/suchai.html) nanosatellite (1U 
[Cubesat](https://en.wikipedia.org/wiki/CubeSat)). SUCHAI was launch into orbit 
in June 2017 and has been working properly for months.

The main idea was to design a highly modular software architecture to help de
development of educational CubeSats projects that are usually composed by
large and heterogeneous teams.

The software architecture is based in the command processor design pattern. Developers
can extend the functionalities adding new commands to the system (with low 
impact in the whole software) or adding new clients that generate available
commands depending on a custom control strategy. Commands and control modules 
can be added or removed with zero impact in the software main functionalities.

Visit http://spel.ing.uchile.cl to get latest news about SUCHAI project
Visit http://www.freertos.org/ to get FreeRTOS source code and documentation
More: http://tesis.uchile.cl/handle/2250/115307

## Key Features

* Designed for medium-range microcontrollers, such as Microchip PIC24 and ATMEL AV32.
* Highly extensible and modular command processor architecture
* Ported to FreeRTOS and Linux 
* Flight inheritance: SUCHAI I (Jun 2017)

## Installation Notes

SUCHAI flight software is designed to run in multiple embedded architectures 
using FreeRTOS. It is also ported to Linux for easy debugging. 
Currently it have been tested in the following OS/Architectures :

* ArchLinux x86_64
* Manjaro x86_64
* Ubuntu 16.04 x86_64

### Linux Installation

#### Library Requirements
Linux installation requires the following libraries:
* pthread 
* sqlite3
* zmq
* libcsp


#### Compile and run
Clone this repository

```bash
git clone https://github.com/spel-uchile/SUCHAI-Flight-Software
cd SUCHAI-Flight-Software
```
Go to folder, locate ```config.h``` file to customize parameters and 
functionalities.

##### Installing LibCSP
LibCSP (Cubesat Space Protocol) implements the CSP protocol stack. It works
on Linux (with ZMQ) and FreeRTOS (I2C or CAN). Follow these step to install
the libcsp:

```bash
cd drivers/Linux/libcsp
sh install_csp.sh
cd -
```

This will download and compile the latest libcsp version from GitHub.


Build the Flight Software with cmake and run. In the root directory execute:

```bash
mkdir build
cd build
cmake ..
make
./SUCHAI_Flight_Software
```

## Contact

Use the [issue tracker](https://github.com/spel-uchile/SUCHAI-Flight-Software/issues) 
to submit questions, requirements and bugs.

Follow the [SPEL team](https://twitter.com/SPEL_UCHILE) at Twitter to get latest 
news about the SUCHAI project