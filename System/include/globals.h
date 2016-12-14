//
// Created by carlos on 02-12-16.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include <osQueue.h>

osQueue dispatcherQueue; /* Commands queue */
osQueue executerCmdQueue; /* Executer commands queue */
osQueue executerStatQueue; /* Executer result queue */

#endif //GLOBALS_H
