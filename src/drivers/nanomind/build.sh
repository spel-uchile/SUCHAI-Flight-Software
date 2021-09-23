#!/usr/bin/env bash
PROGRAM=${1}
cd suchai-drivers-obc
python2 waf configure build $PROGRAM
res=$?
cd -
exit $res
