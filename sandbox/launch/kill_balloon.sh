#!/bin/bash
dpl_id=$(ps aux | grep dpl | grep -v grep | awk '{print $2}')
bmp_id=$(ps aux | grep bmp | grep -v grep | awk '{print $2}')
gps_id=$(ps aux | grep gps | grep -v grep | awk '{print $2}')
ird_id=$(ps aux | grep ird | grep -v grep | awk '{print $2}')
hub_id=$(ps aux | grep hub | grep -v grep | awk '{print $2}')
suchai_id=$(ps aux | grep SUCHAI | grep -v grep | awk '{print $2}')
direwolf_id=$(ps aux | grep direwolf | grep -v grep | awk '{print $2}')

kill $dpl_id $bmp_id $gps_id $ird_id $hub_id $suchai_id $direwolf_id
echo Killed balloon
