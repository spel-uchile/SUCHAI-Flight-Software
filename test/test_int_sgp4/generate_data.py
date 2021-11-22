#!/usr/bin/env python3
"""
This file generate reference date to compare the SUCHAI SGP4 propagator
with results from Python Skyfield SGP4 (https://pypi.org/project/sgp4)

This program calculates R and V for N timestamps.
The TLE used here and in the C-code test must be the same

Execute: python3 generate_data.py
"""
from datetime import datetime
from pytz import utc
from sgp4.api import Satrec, jday
import numpy as np

tle1 = '1 42788U 17036Z   20268.81967680  .00001527  00000-0  65841-4 0  9992'
tle2 = '2 42788  97.2921 320.6805 0012366  42.2734 317.9454 15.23993159181016'
satellite = Satrec.twoline2rv(tle1, tle2)

jdepoch = satellite.jdsatepoch+satellite.jdsatepochF
epoch = (jdepoch-2440587.5)*86400  # Julian date to unix timestamp
# IMPORTANT: This line is to match the C code epoch precision
epoch = int(epoch*1000)/1000

times = np.arange(epoch, epoch+60*100, 60, dtype=int)
tsince = (times - epoch)/60
# times2 = [datetime.fromtimestamp(t, tz=utc) for t in times]
# jdays = [jday(t.year, t.month, t.day, t.hour, t.minute, t.second) for t in times2]
# prop = [np.hstack(satellite.sgp4(jd, fr)) for jd, fr in jdays]
prop = [np.hstack(satellite.sgp4_tsince(t)) for t in tsince]
prop = np.array(prop)
prop[:, 0] = np.array(times)
np.savetxt("data.csv", prop, fmt="%d,%05.8f,%05.8f,%05.8f,%05.8f,%05.8f,%05.8f")