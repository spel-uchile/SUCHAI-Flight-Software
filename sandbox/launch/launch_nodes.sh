#!/bin/bash
FLIGHTSOFTWARE=~/Spel/SUCHAI-Flight-Software/build_linux
cd $FLIGHTSOFTWARE
./SUCHAI_Flight_Software >  ~/Spel/mission-history/flight_software-"$(date "+%FT%T")".log 2>&1 &

cd ../sandbox
python3 zmqhub.py --mon  > ~/Spel/mission-history/zmq_hub-"$(date "+%FT%T")".log 2>&1 &

SUBMODULES=~/Spel/submodules/sw/bus-arquitecture
cd $SUBMODULES
python3 bmp/bmp_com.py  > ~/Spel/mission-history/bmp_com-"$(date "+%FT%T")".log 2>&1 &
python3 dpl/dpl_com.py > ~/Spel/mission-history/dpl_com-"$(date "+%FT%T")".log 2>&1 &
python gps/gps_com.py > ~/Spel/mission-history/gps_com-"$(date "+%FT%T")".log 2>&1 &
python3 iridium/ird_com.py > ~/Spel/mission-history/ird_com-"$(date "+%FT%T")".log 2>&1 &

cd  ~/Spel
