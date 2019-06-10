#!/usr/bin/python3

__author__ = 'gdiaz'

# BMP180 ZMQ COM INTERFACE

"""Provides a high level interface over ZMQ for data exchange.
"""

"""Description
    BMP comunication: node1

    console:  send data to node1

    data[bmp]-->console[PUB]-> node1

"""

import zmq
import sys
import time
import re
import argparse
import json
import struct

from threading import Thread
from time import sleep

#sys.path.append('../')
#print(sys.path)

# Get Nodes and Ports Parameters
#from nodes.node_list import NODE_BMP, NODE_OBC, CSP_PORT_APPS
with open('node_list.json', encoding='utf-8') as data_file:
    data = json.load(data_file)

NODE_BMP = data["nodes"]["bmp"]
NODE_OBC = data["nodes"]["obc"]
CSP_PORT_APPS = data["ports"]["telemetry"]

#define commands
GET_DATA = "get_prs_data"

class BmpComInterface:
    def __init__(self, sim):
        # sensor arguments
        self.sensor_bmp = BMP085.BMP085() if not sim else None
        self.timestamp = 0
        self.temperature = 0        #float
        self.pressure = 0           #float
        self.altitude = 0           #float
        #com args
        self.node = chr(int(NODE_BMP)).encode("ascii", "replace")
        self.node_dest = NODE_OBC
        self.port_csp = CSP_PORT_APPS
        self.prompt = "[node({}) port({})] <message>: "

    def update_data(self):
        while True:
            bmp_exist = self.sensor_bmp is not None
            self.timestamp = int(time.time())
            self.temperature = self.sensor_bmp.read_temperature() if bmp_exist else 1.0
            self.pressure = self.sensor_bmp.read_pressure() if bmp_exist else 2.0
            self.altitude = self.sensor_bmp.read_altitude() if bmp_exist else 3.0
            time.sleep(0.25)

    def console(self, ip="localhost", in_port_tcp=8002, out_port_tcp=8001):
        """ Send messages to node """
        ctx = zmq.Context()
        pub = ctx.socket(zmq.PUB)
        sub = ctx.socket(zmq.SUB)
        sub.setsockopt(zmq.SUBSCRIBE, self.node)
        pub.connect('tcp://{}:{}'.format(ip, out_port_tcp))
        sub.connect('tcp://{}:{}'.format(ip, in_port_tcp))
        print('Start Atmospheric Intreface as node:" {},'.format(int.from_bytes(self.node, byteorder='little')))

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
            cmd = data.decode("ascii", "replace")

            if cmd == GET_DATA:
                #update data
                print('\nMeasurements:')
                print('\tPressure: {}'.format(self.pressure))
                print('\tTemperature: {},'.format(self.temperature))
                print('\tAltitude: {}'.format(self.altitude))
                # build msg
                #          Prio   SRC   DST    DP   SP  RES HXRC
                header_ = "{:02b}{:05b}{:05b}{:06b}{:06b}00000000"

                prompt = self.prompt.format(self.node_dest, self.port_csp)
                # Get CSP header_ and data
                hdr = header_.format(1, int.from_bytes(self.node, byteorder='little'), self.node_dest, self.port_csp, 63)

                # Build CSP message
                hdr_b = re.findall("........",hdr)[::-1]
                # print("con:", hdr_b, ["{:02x}".format(int(i, 2)) for i in hdr_b])
                hdr = bytearray([int(i,2) for i in hdr_b])

                # join data
                n_frame = 0
                # BMP Telemetry Type
                fr_type = 15
                n_samples = 1
                data_ = bytearray(struct.pack('hhi', n_frame, fr_type, n_samples))
                data_ = data_ + \
                    struct.pack('Ifff', self.timestamp, self.pressure, self.temperature, self.altitude)

                msg = bytearray([int(self.node_dest),]) + hdr + data_
                # send data to OBC node
                try:
                    pub.send(msg)
                except Exception as e:
                    pass
            cmd = -1
            #sys.stdout.flush()

def get_parameters():
    """ Parse command line parameters """
    parser = argparse.ArgumentParser()

    parser.add_argument("-n", "--node", default=10, help="Node address")
    parser.add_argument("-d", "--ip", default="localhost", help="Hub IP address")
    parser.add_argument("-i", "--in_port", default="8001", help="Hub Input port")
    parser.add_argument("-o", "--out_port", default="8002", help="Hub Output port")
    parser.add_argument("--nmon", action="store_false", help="Disable monitor task")
    parser.add_argument("--ncon", action="store_false", help="Disable console task")
    parser.add_argument("--sim", action="store_true", help="Make simulation available for bmp driver")

    return parser.parse_args()

if __name__ == '__main__':
    # Get arguments
    args = get_parameters()

    if not args.sim:
        import Adafruit_BMP.BMP085 as BMP085

    bmp = BmpComInterface(sim=args.sim)

    tasks = []

    # Create a update data thread
    data_th = Thread(target=bmp.update_data)
    # data_th.daemon = True
    tasks.append(data_th)
    data_th.start()

    if args.ncon:
        # Create a console socket
        console_th = Thread(target=bmp.console, args=(args.ip, args.out_port, args.in_port))
        # console_th.daemon = True
        tasks.append(console_th)
        console_th.start()

    for th in tasks:
        th.join()
