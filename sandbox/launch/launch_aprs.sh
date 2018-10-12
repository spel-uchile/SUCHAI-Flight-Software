#!/bin/bash
SUBMODULES=~/Spel/submodules/sw/bus-arquitecture
cd $SUBMODULES
direwolf -t 0 -c aprs/aprs_telem.conf  > ~/Spel/aprs.log 2>&1
cd  ~/Spel
