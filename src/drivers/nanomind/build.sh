#!/usr/bin/env bash
cd a3200-sdk-lite-v1.2/
python2 waf configure build $1
cd -
