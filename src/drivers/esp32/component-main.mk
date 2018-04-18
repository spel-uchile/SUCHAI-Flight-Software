#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the
# src/ directory, compile them and link them into lib(subdirectory_name).a
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#
COMPONENT_OBJS := cmdDRP.o cmdOBC.o globals.o main.o repoData.o taskConsole.o\
taskExecuter.o taskHousekeeping.o taskWatchdog.o cmdConsole.o cmdFP.o repoCommand.o\
taskDispatcher.o taskFlightPlan.o taskInit.o utils.o

