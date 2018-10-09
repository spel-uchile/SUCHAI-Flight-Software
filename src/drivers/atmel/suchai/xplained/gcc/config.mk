#
# Adapted from Atmel's ASF
#

# Get the project name
MKFILE_PATH := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BOARD_PATH := $(abspath $(addsuffix /.., $(MKFILE_PATH)))
PROJ_PATH := $(abspath $(addsuffix /.., $(BOARD_PATH)))

BOARD_NAME := $(shell basename $(BOARD_PATH))
PROJ_NAME := $(shell basename $(PROJ_PATH))

# Relative to ASF_PATH, because Atmel's Makefile doesn't like absolute
# include paths. >:C
PROJ_ROOT := $(addprefix avr32/applications/, $(PROJ_NAME))

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr2

# Target part: none, ap7xxx or uc3xxxxx
PART = uc3a3256

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = int

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = $(PROJ_NAME).elf

# List of C source files.
CSRCS = \
       $(PROJ_ROOT)/system/main.c                         \
       $(PROJ_ROOT)/system/cmdConsole.c                   \
       $(PROJ_ROOT)/system/cmdDRP.c                       \
       $(PROJ_ROOT)/system/cmdFP.c                        \
       $(PROJ_ROOT)/system/cmdOBC.c                       \
       $(PROJ_ROOT)/system/cmdCOM.c                       \
       $(PROJ_ROOT)/system/cmdEPS.c                       \
       $(PROJ_ROOT)/system/repoCommand.c                  \
       $(PROJ_ROOT)/system/repoData.c                     \
       $(PROJ_ROOT)/system/taskWatchdog.c                 \
       $(PROJ_ROOT)/system/taskInit.c	                  \
       $(PROJ_ROOT)/system/taskConsole.c                  \
       $(PROJ_ROOT)/system/taskDispatcher.c               \
       $(PROJ_ROOT)/system/taskExecuter.c                 \
       $(PROJ_ROOT)/system/taskFlightPlan.c               \
       $(PROJ_ROOT)/system/taskHousekeeping.c             \
       $(PROJ_ROOT)/system/taskCommunications.c           \
       $(PROJ_ROOT)/util/hexdump.c                        \
       $(PROJ_ROOT)/util/memcheck.c                       \
       $(PROJ_ROOT)/util/init.c                           \
       $(PROJ_ROOT)/lowlevel/port.c                       \
       $(PROJ_ROOT)/os/FreeRTOS/osDelay.c                 \
       $(PROJ_ROOT)/os/FreeRTOS/osQueue.c                 \
       $(PROJ_ROOT)/os/FreeRTOS/osScheduler.c             \
       $(PROJ_ROOT)/os/FreeRTOS/osSemphr.c                \
       $(PROJ_ROOT)/os/FreeRTOS/osThread.c                \
       avr32/boards/uc3_a3_xplained/init.c                \
       avr32/boards/uc3_a3_xplained/led.c                 \
       avr32/drivers/flashc/flashc.c                      \
       avr32/drivers/ebi/sdramc/sdramc.c                  \
       avr32/drivers/intc/intc.c                          \
       avr32/drivers/gpio/gpio.c                          \
       avr32/drivers/usart/usart.c                        \
       avr32/drivers/tc/tc.c                              \
       avr32/drivers/pm/pm.c                              \
       avr32/drivers/pm/pm_conf_clocks.c                  \
       avr32/drivers/pm/power_clocks_lib.c                \
       avr32/drivers/usbb/usbb_device.c                   \
       avr32/drivers/spi/spi.c                            \
       avr32/drivers/twim/twim.c                            \
       avr32/drivers/rtc/rtc.c                            \
       avr32/drivers/pdca/pdca.c                          \
       avr32/drivers/wdt/wdt.c                            \
       common/utils/stdio/read.c                          \
       common/utils/stdio/write.c                         \
       common/utils/stdio/stdio_usb/stdio_usb.c           \
       common/services/clock/uc3a3_a4/sysclk.c            \
       common/services/sleepmgr/uc3/sleepmgr.c            \
       common/services/usb/class/cdc/device/udi_cdc.c     \
       common/services/usb/class/cdc/device/udi_cdc_desc.c\
       common/services/usb/udc/udc.c                      \
       common/services/spi/uc3_spi/spi_master.c           \
       thirdparty/freertos/freertos-7.0.0/source/croutine.c     \
       thirdparty/freertos/freertos-7.0.0/source/list.c         \
       thirdparty/freertos/freertos-7.0.0/source/queue.c        \
       thirdparty/freertos/freertos-7.0.0/source/tasks.c        \
       thirdparty/freertos/freertos-7.0.0/source/timers.c       \
       thirdparty/freertos/freertos-7.0.0/source/portable/memmang/heap_3.c      \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/avr32_uc3/read.c  \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/avr32_uc3/write.c

# List of assembler source files.
ASSRCS = \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/avr32_uc3/exception.S \
       $(PROJ_ROOT)/lowlevel/startup_uc3.S                \
       avr32/utils/startup/trampoline_uc3.S

#List of include paths.
INC_PATH = \
       $(PROJ_ROOT)/util                                  \
       $(PROJ_ROOT)/system/include                        \
       $(PROJ_ROOT)/os/include                            \
       $(PROJ_ROOT)/libcsp/include                        \
       $(PROJ_ROOT)/libcsp/libcsp/include                 \
       $(PROJ_ROOT)/libcsp/libcsp/include/csp              \
       $(PROJ_ROOT)/libcsp/libcsp/include/csp/interfaces   \
       $(PROJ_ROOT)/libcsp/libcsp/include/arch            \
       $(PROJ_ROOT)/libcsp/libcsp/include/drivers         \
       $(PROJ_ROOT)/libcsp/libcsp/src                     \
       $(PROJ_ROOT)/libcsp/libcsp/src/transport           \
       $(PROJ_ROOT)/libutil/include/                      \
       $(PROJ_ROOT)/$(BOARD_NAME)                         \
       $(PROJ_ROOT)/$(BOARD_NAME)/gcc                     \
       $(PROJ_ROOT)/gomspace/drivers/include               \
       avr32/boards                                       \
       avr32/boards/uc3_a3_xplained                       \
       avr32/drivers/cpu/cycle_counter                    \
       avr32/drivers/flashc                               \
       avr32/drivers/ebi/sdramc                           \
       avr32/drivers/gpio                                 \
       avr32/drivers/intc                                 \
       avr32/drivers/usart                                \
       avr32/drivers/tc                                   \
       avr32/drivers/pm                                   \
       avr32/drivers/usbb                                 \
       avr32/drivers/spi                                  \
       avr32/drivers/twim                                  \
       avr32/drivers/rtc                                  \
       avr32/drivers/pdca                                 \
       avr32/drivers/wdt                                  \
       avr32/drivers/cpu                                  \
       avr32/utils                                        \
       avr32/utils/preprocessor                           \
       common/boards                                      \
       common/services/clock                              \
       common/services/delay                              \
       common/services/sleepmgr                           \
       common/services/serial                             \
       common/services/usb                                \
       common/services/usb/class/cdc                      \
       common/services/usb/class/cdc/device               \
       common/services/usb/udc                            \
       common/services/spi                                \
       common/services/spi/uc3_spi                        \
       common/utils                                       \
       common/utils/stdio                                 \
       common/utils/stdio/stdio_usb                       \
       common/utils/stdio/stdio_serial                    \
       thirdparty/freertos/freertos-7.0.0/include         \
       thirdparty/freertos/freertos-7.0.0/module_config   \
       thirdparty/freertos/freertos-7.0.0/source/include  \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/avr32_uc3/

# Additional search paths for libraries.
LIB_PATH =

# List of libraries to use during linking.
LIBS =

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT = $(PROJ_ROOT)/$(BOARD_NAME)/gcc/link_uc3a3256.lds

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS =

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -Os

# Extra flags to use when archiving.
ARFLAGS =

# Extra flags to use when assembling.
ASFLAGS =

# Extra flags to use when compiling.
CFLAGS =

# Extra flags to use when preprocessing.
#
# Preprocessor symbol definitions
#   To add a definition use the format "-D name[=definition]".
#   To cancel a definition use the format "-U name".
#
# The most relevant symbols to define for the preprocessor are:
#   BOARD      Target board in use, see boards/board.h for a list.
#   EXT_BOARD  Optional extension board in use, see boards/board.h for a list.
CPPFLAGS = \
       -D BOARD=UC3_A3_XPLAINED

# Extra flags to use when linking
LDFLAGS = \
       -nostartfiles -Wl,-e,_trampoline -nostartfiles

# Pre- and post-build commands
PREBUILD_CMD =
POSTBUILD_CMD =
