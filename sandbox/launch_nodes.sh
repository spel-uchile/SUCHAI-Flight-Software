#!/bin/bash
FLIGHTSOFTWARE=~/Spel/SUCHAI-Flight-Software/build_linux
cd $FLIGHTSOFTWARE
./SUCHAI_Flight_Software >  ~/Spel/flight_software.log 2>&1 &

cd ../sandbox
python3 zmqhub.py --mon  > ~/Spel/zmq_hub.log 2>&1 &

SUBMODULES=~/Spel/balloon_experiment/sw/bus-arquitecture
cd $SUBMODULES
python3 bmp/bmp_com.py  > ~/Spel/bmp_com.log 2>&1 &
python3 dpl/dpl_com.py > ~/Spel/dpl_com.log 2>&1 &
python gps/gps_com.py > ~/Spel/gps_com.log 2>&1 &
python3 iridium/ird_com.py > ~/Spel/ird_com.log 2>&1 &

cd  ~/Spel
