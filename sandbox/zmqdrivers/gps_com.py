#!/usr/bin/python

__author__ = 'gdiaz'

# GPS ZMQ COM INTERFACE

"""Provides a high level interface over ZMQ for data exchange.
"""

"""Description
    GPS comunication: node1

    console:  send data to node1

    data[gps]-->console[PUB]-> node1

"""

import zmq
import sys
import time
import re
import argparse
import codecs
import math
import json
import struct

from threading import Thread
from time import sleep
from gps import *
from struct import *

sys.path.append('../')

# Get Nodes and Ports Parameters
with open('node_list.json') as data_file:
    data = json.load(data_file)

NODE_GPS = data["nodes"]["gps"]
NODE_OBC = data["nodes"]["obc"]
CSP_PORT_APPS = data["ports"]["telemetry"]

#define commands
GET_DATA = "get_gps_data"

class GpsComInterface:
    def __init__(self, sim):
        # gps arguments
        self.gps_handler = gps(mode=WATCH_ENABLE) if not sim else None #starting the stream of info
        
        self.latitude = 0           #float
        self.longitude = 0          #float
        self.time_utc = 0           #str
        self.fix_time = 0           #float
        self.altitude = 0           #float
        self.speed_horizontal = 0   #float
        self.speed_vertical = 0     #float
        self.mode = 0               #int
        self.satellites = 0         #int
        #com args
        self.node = chr(int(NODE_GPS)).encode("ascii", "replace")
        self.node_dest = NODE_OBC
        self.port_csp = CSP_PORT_APPS
        self.prompt = "[node({}) port({})] <message>: "

    def check_nan(self, num, id):
        #print("GPS_DEBUG:"+str(type(num))+"    "+str(num))
        if num == None:
            return "NoTimeStamp"
        try:
            if math.isnan(num):
                return -1
        except:
            #print num, id
            if id==3:#TODO: chech this!!!
                return -1
            return num
        return num

    def update_data(self):
        handler_exist = self.gps_handler is not None
        while True:
            self.latitude = self.check_nan(self.gps_handler.fix.latitude, 1.0) if handler_exist else 1.0
            self.longitude = self.check_nan(self.gps_handler.fix.longitude, 2.0) if handler_exist else 2.0
            self.time_utc = self.gps_handler.utc if handler_exist else int(time.time())
            self.fix_time = self.check_nan(self.gps_handler.fix.time, 3) if handler_exist else 3
            self.altitude = self.check_nan(self.gps_handler.fix.altitude, 4.0) if handler_exist else 4.0
            self.speed_horizontal = self.check_nan(self.gps_handler.fix.speed, 5.0) if handler_exist else 5.0
            self.speed_vertical = self.check_nan(self.gps_handler.fix.climb, 6.0) if handler_exist else 6.0
            self.mode = self.check_nan(self.gps_handler.fix.mode, 7) if handler_exist else 7
            self.satellites = self.check_nan(len(self.gps_handler.satellites), 8) if handler_exist else 8
            time.sleep(0.1)
            if handler_exist:
                self.gps_handler.next()

    def console(self, ip="localhost", in_port_tcp=8002, out_port_tcp=8001):
        """ Send messages to node """
        ctx = zmq.Context()
        pub = ctx.socket(zmq.PUB)
        sub = ctx.socket(zmq.SUB)
        sub.setsockopt(zmq.SUBSCRIBE, self.node)
        pub.connect('tcp://{}:{}'.format(ip, out_port_tcp))
        sub.connect('tcp://{}:{}'.format(ip, in_port_tcp))
        print('Start GPS Intreface as node: {}'.format(int(codecs.encode(self.node, 'hex'), 16)))

        while True:

            frame = sub.recv_multipart()[0]
            header_a = ["{:02x}".format(i) for i in frame[1:5]]
            header = "0x"+"".join(header_a[::-1])
            data = frame[5:]
            try:
                csp_header = parse_csp(header)
            except:
                csp_header = ""
            data = data[:-1]
            print('\nMON:', frame)
            print('\tHeader: {},'.format(csp_header))
            print('\tData: {}'.format(data))

            cmd = data.decode("utf-8")

            if cmd == GET_DATA:
                #update data
                print('\nMeasurements:')
                print('\tTime_utc: {}'.format(self.time_utc))
                print('\tFix_time: {}'.format(self.fix_time))
                print('\tLatitude: {},'.format(self.latitude))
                print('\tLongitude: {}'.format(self.longitude))
                print('\tAltitude: {}'.format(self.altitude))
                print('\tSpeed_horizontal: {}'.format(self.speed_horizontal))
                print('\tSatellite number: {}'.format(self.satellites))
                print('\tMode: {}'.format(self.mode))
                # build msg
                #          Prio   SRC   DST    DP   SP  RES HXRC
                header_ = "{:02b}{:05b}{:05b}{:06b}{:06b}00000000"

                prompt = self.prompt.format(self.node_dest, self.port_csp)
                # Get CSP header_ and data
                hdr = header_.format(1, int(codecs.encode(self.node, 'hex'), 16), self.node_dest, self.port_csp, 63)

                # Build CSP message
                hdr_b = re.findall("........", hdr)[::-1]
                # print("con:", hdr_b, ["{:02x}".format(int(i, 2)) for i in hdr_b])
                hdr = bytearray([int(i, 2) for i in hdr_b])

                n_frame = 0
                # GPS Telemetry Type
                fr_type = 14
                n_samples = 1
                data_ = bytearray(struct.pack('h', n_frame) + struct.pack('h', fr_type) + struct.pack('i', n_samples))
                # values = [self.time_utc, self.latitude, self.longitude, self.altitude, self.speed_horizontal, self.speed_vertical, self.satellites, self.mode]
                data_ = data_ + \
                         struct.pack('I', self.time_utc) + \
                         struct.pack('f', self.latitude) + \
                         struct.pack('f', self.longitude) + \
                         struct.pack('f', self.altitude) + \
                         struct.pack('f', self.speed_horizontal) + \
                         struct.pack('f', self.speed_vertical) + \
                         struct.pack('i', self.satellites) + \
                         struct.pack('i', self.mode)

                msg = bytearray([int(self.node_dest),]) + hdr + data_
                print('\nMessage:', msg)
                # send data to OBC node
                try:
                    pub.send(msg)
                except Exception as e:
                    pass
            cmd = -1

def get_parameters():
    """ Parse command line parameters """
    parser = argparse.ArgumentParser()

    parser.add_argument("-n", "--node", default=10, help="Node address")
    parser.add_argument("-d", "--ip", default="localhost", help="Hub IP address")
    parser.add_argument("-i", "--in_port", default="8001", help="Hub Input port")
    parser.add_argument("-o", "--out_port", default="8002", help="Hub Output port")
    parser.add_argument("--nmon", action="store_false", help="Disable monitor task")
    parser.add_argument("--ncon", action="store_false", help="Disable console task")
    parser.add_argument("--sim", action="store_true", help="Make available simulation of gps")

    return parser.parse_args()

if __name__ == '__main__':
    # Get arguments
    args = get_parameters()

    gps = GpsComInterface(sim=args.sim)

    tasks = []

    # Create a update data thread
    data_th = Thread(target=gps.update_data)
    # data_th.daemon = True
    tasks.append(data_th)
    data_th.start()

    if args.ncon:
        # Create a console socket
        console_th = Thread(target=gps.console, args=(args.ip, args.out_port, args.in_port))
        # console_th.daemon = True
        tasks.append(console_th)
        console_th.start()

    for th in tasks:
        th.join()
