#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-Sim.mk)" "nbproject/Makefile-local-Sim.mk"
include nbproject/Makefile-local-Sim.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=Sim
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/SUCHAI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/SUCHAI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1728494092/taskTest.o ${OBJECTDIR}/_ext/2014179570/main.o ${OBJECTDIR}/_ext/2067230309/croutine.o ${OBJECTDIR}/_ext/2067230309/list.o ${OBJECTDIR}/_ext/2067230309/queue.o ${OBJECTDIR}/_ext/2067230309/tasks.o ${OBJECTDIR}/_ext/2067230309/timers.o ${OBJECTDIR}/_ext/1932347678/heap_1.o ${OBJECTDIR}/_ext/1963833021/port.o ${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1728494092/taskTest.o.d ${OBJECTDIR}/_ext/2014179570/main.o.d ${OBJECTDIR}/_ext/2067230309/croutine.o.d ${OBJECTDIR}/_ext/2067230309/list.o.d ${OBJECTDIR}/_ext/2067230309/queue.o.d ${OBJECTDIR}/_ext/2067230309/tasks.o.d ${OBJECTDIR}/_ext/2067230309/timers.o.d ${OBJECTDIR}/_ext/1932347678/heap_1.o.d ${OBJECTDIR}/_ext/1963833021/port.o.d ${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1728494092/taskTest.o ${OBJECTDIR}/_ext/2014179570/main.o ${OBJECTDIR}/_ext/2067230309/croutine.o ${OBJECTDIR}/_ext/2067230309/list.o ${OBJECTDIR}/_ext/2067230309/queue.o ${OBJECTDIR}/_ext/2067230309/tasks.o ${OBJECTDIR}/_ext/2067230309/timers.o ${OBJECTDIR}/_ext/1932347678/heap_1.o ${OBJECTDIR}/_ext/1963833021/port.o ${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Sim.mk dist/${CND_CONF}/${IMAGE_TYPE}/SUCHAI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ256GA110
MP_LINKER_FILE_OPTION=,--script=p24FJ256GA110.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1728494092/taskTest.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/System/taskTest.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1728494092 
	@${RM} ${OBJECTDIR}/_ext/1728494092/taskTest.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/System/taskTest.c"  -o ${OBJECTDIR}/_ext/1728494092/taskTest.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1728494092/taskTest.o.d"        -g -D__DEBUG   -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728494092/taskTest.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2014179570/main.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2014179570 
	@${RM} ${OBJECTDIR}/_ext/2014179570/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/main.c"  -o ${OBJECTDIR}/_ext/2014179570/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2014179570/main.o.d"        -g -D__DEBUG   -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/2014179570/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2067230309/croutine.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/croutine.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2067230309 
	@${RM} ${OBJECTDIR}/_ext/2067230309/croutine.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/croutine.c"  -o ${OBJECTDIR}/_ext/2067230309/croutine.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2067230309/croutine.o.d"        -g -D__DEBUG   -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/2067230309/croutine.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2067230309/list.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/list.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2067230309 
	@${RM} ${OBJECTDIR}/_ext/2067230309/list.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/list.c"  -o ${OBJECTDIR}/_ext/2067230309/list.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2067230309/list.o.d"        -g -D__DEBUG   -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/2067230309/list.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2067230309/queue.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2067230309 
	@${RM} ${OBJECTDIR}/_ext/2067230309/queue.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/queue.c"  -o ${OBJECTDIR}/_ext/2067230309/queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2067230309/queue.o.d"        -g -D__DEBUG   -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/2067230309/queue.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2067230309/tasks.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2067230309 
	@${RM} ${OBJECTDIR}/_ext/2067230309/tasks.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/tasks.c"  -o ${OBJECTDIR}/_ext/2067230309/tasks.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2067230309/tasks.o.d"        -g -D__DEBUG   -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/2067230309/tasks.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2067230309/timers.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2067230309 
	@${RM} ${OBJECTDIR}/_ext/2067230309/timers.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/timers.c"  -o ${OBJECTDIR}/_ext/2067230309/timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2067230309/timers.o.d"        -g -D__DEBUG   -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/2067230309/timers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1932347678/heap_1.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/portable/MemMang/heap_1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1932347678 
	@${RM} ${OBJECTDIR}/_ext/1932347678/heap_1.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/portable/MemMang/heap_1.c"  -o ${OBJECTDIR}/_ext/1932347678/heap_1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1932347678/heap_1.o.d"        -g -D__DEBUG   -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1932347678/heap_1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1963833021/port.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/port.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1963833021 
	@${RM} ${OBJECTDIR}/_ext/1963833021/port.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/port.c"  -o ${OBJECTDIR}/_ext/1963833021/port.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1963833021/port.o.d"        -g -D__DEBUG   -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1963833021/port.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1728494092/taskTest.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/System/taskTest.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1728494092 
	@${RM} ${OBJECTDIR}/_ext/1728494092/taskTest.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/System/taskTest.c"  -o ${OBJECTDIR}/_ext/1728494092/taskTest.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1728494092/taskTest.o.d"        -g -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728494092/taskTest.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2014179570/main.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2014179570 
	@${RM} ${OBJECTDIR}/_ext/2014179570/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/main.c"  -o ${OBJECTDIR}/_ext/2014179570/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2014179570/main.o.d"        -g -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/2014179570/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2067230309/croutine.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/croutine.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2067230309 
	@${RM} ${OBJECTDIR}/_ext/2067230309/croutine.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/croutine.c"  -o ${OBJECTDIR}/_ext/2067230309/croutine.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2067230309/croutine.o.d"        -g -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/2067230309/croutine.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2067230309/list.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/list.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2067230309 
	@${RM} ${OBJECTDIR}/_ext/2067230309/list.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/list.c"  -o ${OBJECTDIR}/_ext/2067230309/list.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2067230309/list.o.d"        -g -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/2067230309/list.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2067230309/queue.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2067230309 
	@${RM} ${OBJECTDIR}/_ext/2067230309/queue.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/queue.c"  -o ${OBJECTDIR}/_ext/2067230309/queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2067230309/queue.o.d"        -g -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/2067230309/queue.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2067230309/tasks.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2067230309 
	@${RM} ${OBJECTDIR}/_ext/2067230309/tasks.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/tasks.c"  -o ${OBJECTDIR}/_ext/2067230309/tasks.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2067230309/tasks.o.d"        -g -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/2067230309/tasks.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2067230309/timers.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2067230309 
	@${RM} ${OBJECTDIR}/_ext/2067230309/timers.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/timers.c"  -o ${OBJECTDIR}/_ext/2067230309/timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2067230309/timers.o.d"        -g -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/2067230309/timers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1932347678/heap_1.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/portable/MemMang/heap_1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1932347678 
	@${RM} ${OBJECTDIR}/_ext/1932347678/heap_1.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/portable/MemMang/heap_1.c"  -o ${OBJECTDIR}/_ext/1932347678/heap_1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1932347678/heap_1.o.d"        -g -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1932347678/heap_1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1963833021/port.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/port.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1963833021 
	@${RM} ${OBJECTDIR}/_ext/1963833021/port.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/port.c"  -o ${OBJECTDIR}/_ext/1963833021/port.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1963833021/port.o.d"        -g -omf=elf -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -fomit-frame-pointer -I".." -I"../Drivers/include" -I"../System/include" -I"../Payloads/Cmd/include" -I"../Payloads/Drivers/include" -I"../FreeRTOS/Source/include" -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/" -DMPLAB_PIC24_PORT -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1963833021/port.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/portasm_PIC24.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1963833021 
	@${RM} ${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/portasm_PIC24.S"  -o ${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o.d"  -D__DEBUG   -omf=elf -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC" -Wa,-MD,"${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o.d" "${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o: /mnt/datos/Documentos/Ingenieria/Electrica/Suchai\ Software/FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/portasm_PIC24.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1963833021 
	@${RM} ${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  "/mnt/datos/Documentos/Ingenieria/Electrica/Suchai Software/FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/portasm_PIC24.S"  -o ${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o.d"  -omf=elf -I"../FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC" -Wa,-MD,"${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o.d" "${OBJECTDIR}/_ext/1963833021/portasm_PIC24.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/SUCHAI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/SUCHAI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG   -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,--report-mem$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/SUCHAI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/SUCHAI.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,--report-mem$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}/xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/SUCHAI.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Sim
	${RM} -r dist/Sim

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
