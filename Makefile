###
# @file MakeFile
# @author Ignacio Ibañez Aliaga 
# @date 26-10-2016
# @copyright GNU Public License.
# 
# MakeFile for Linux
# 
###

WARN=-Wall

#se ejecuta como principal la regla programa
all: suchai clean

#se comienzan a escribir las reglas
suchai: main.o taskTest.o cmdDRP.o cmdOBC.o repoCommand.o repoData.o taskDispatcher.o taskExecuter.o taskHousekeeping.o osThread.o osDelay.o osScheduler.o osQueue.o osSemphr.o pthread_queue.o
	gcc ${WARN} -o suchai main.o taskTest.o cmdDRP.o cmdOBC.o repoCommand.o repoData.o taskDispatcher.o taskExecuter.o taskHousekeeping.o osThread.o osDelay.o osScheduler.o osQueue.o osSemphr.o pthread_queue.o -lpthread

main.o: main.c SUCHAI_config.h System/include/taskTest.h System/include/taskDispatcher.h System/include/taskExecuter.h System/include/taskHousekeeping.h OS/include/osThread.h OS/include/osQueue.h OS/include/osScheduler.h OS/include/osSemphr.h
	gcc ${WARN} -c main.c

taskTest.o: System/taskTest.c System/include/taskTest.h OS/include/osDelay.h OS/include/osQueue.h
	gcc ${WARN} -c System/taskTest.c 

taskDispatcher.o: System/taskDispatcher.c System/include/taskHousekeeping.h
	gcc ${WARN} -c System/taskDispatcher.c

taskExecuter.o: System/taskExecuter.c System/include/taskExecuter.h
	gcc ${WARN} -c System/taskExecuter.c

taskHousekeeping.o: System/taskHousekeeping.c System/include/taskHousekeeping.h System/include/cmdDRP.h OS/include/osQueue.h OS/include/osDelay.h
	gcc ${WARN} -c System/taskHousekeeping.c

cmdDRP.o: System/cmdDRP.c System/include/cmdDRP.h System/include/repoData.h
	gcc ${WARN} -c System/cmdDRP.c

cmdOBC.o: System/cmdOBC.c System/include/cmdOBC.h
	gcc ${WARN} -c System/cmdOBC.c

repoCommand.o: System/repoCommand.c System/include/repoCommand.h
	gcc ${WARN} -c System/repoCommand.c

repoData.o: System/repoData.c System/include/repoData.h OS/include/osSemphr.h
	gcc ${WARN} -c System/repoData.c

osThread.o: OS/Linux/osThread.c OS/include/osThread.h
	gcc ${WARN} -c OS/Linux/osThread.c

osDelay.o: OS/Linux/osDelay.c OS/include/osDelay.h
	gcc ${WARN} -c OS/Linux/osDelay.c

osScheduler.o: OS/Linux/osScheduler.c OS/include/osScheduler.h
	gcc ${WARN} -c OS/Linux/osScheduler.c

osQueue.o: OS/Linux/osQueue.c OS/include/osQueue.h OS/include/pthread_queue.h
	gcc ${WARN} -c OS/Linux/osQueue.c

osSemphr.o: OS/Linux/osSemphr.c OS/include/osSemphr.h
	gcc ${WARN} -c OS/Linux/osSemphr.c

pthread_queue.o: OS/Linux/pthread_queue.c OS/include/pthread_queue.h
	gcc ${WARN} -c OS/Linux/pthread_queue.c

#borrar los archivos con el comando "make clean"
clean:
	rm -f *.o
