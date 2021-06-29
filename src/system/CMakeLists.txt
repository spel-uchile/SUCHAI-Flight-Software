# Base source files
set(SOURCE_FILES_BASE
        globals.c
        cmdDRP.c
        cmdOBC.c
        repoCommand.c
        repoData.c
        repoDataSchema.c
        taskDispatcher.c
        taskExecuter.c
        taskInit.c
        taskWatchdog.c
        mainFS.c
)
# Optional source files
if(SCH_CON_ENABLED)
    list(APPEND SOURCE_FILES_OPT ${CMAKE_CURRENT_LIST_DIR}/taskConsole.c ${CMAKE_CURRENT_LIST_DIR}/cmdConsole.c)
endif()
if(SCH_COMM_ENABLE)
    list(APPEND SOURCE_FILES_OPT ${CMAKE_CURRENT_LIST_DIR}/taskCommunications.c ${CMAKE_CURRENT_LIST_DIR}/cmdCOM.c ${CMAKE_CURRENT_LIST_DIR}/cmdTM.c)
endif()
if(SCH_FP_ENABLED)
    list(APPEND SOURCE_FILES_OPT ${CMAKE_CURRENT_LIST_DIR}/taskFlightPlan.c ${CMAKE_CURRENT_LIST_DIR}/cmdFP.c)
endif()
SET(SOURCE_FILES_SYSTEM ${SOURCE_FILES_BASE} ${SOURCE_FILES_OPT})

add_library(suchai-fs-core ${SOURCE_FILES_SYSTEM})
target_include_directories(suchai-fs-core PUBLIC ${PROJECT_SOURCE_DIR}/include include)