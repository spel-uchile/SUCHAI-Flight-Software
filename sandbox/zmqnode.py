import re
import zmq
import time
import argparse
from threading import Thread

prompt = "[node({}) port({})] <message>: "

def parse_csp(hdrhex):
    hdrhex = int(hdrhex, 16)
    header = "S {}, D {}, Dp {}, Sp {}, Pr {}, HMAC {} XTEA {} RDP {} CRC32 {}".format(
        (hdrhex >> 25) & 0x1f,
        (hdrhex >> 20) & 0x1f,
        (hdrhex >> 14) & 0x3f,
        (hdrhex >> 8) & 0x3f,
        (hdrhex >> 30) & 0x03,
        "Yes" if ((hdrhex >> 3) & 0x01) else "No",
        "Yes" if ((hdrhex >> 2) & 0x01) else "No",
        "Yes" if ((hdrhex >> 1) & 0x01) else "No",
        "Yes" if ((hdrhex >> 0) & 0x01) else "No")

    return header


def monitor(port="8002", ip="localhost", node=b''):
    """ Read messages from node(s) """
    if node != b'':
        node = chr(int(node)).encode("ascii", "replace")
    ctx = zmq.Context(1)
    sock = ctx.socket(zmq.SUB)
    sock.setsockopt(zmq.SUBSCRIBE, node)
    sock.connect('tcp://{}:{}'.format(ip, port))

    while True:
        frame = sock.recv_multipart()[0]
        header_a = ["{:02x}".format(i) for i in frame[1:5]]
        header = "0x"+"".join(header_a[::-1])
        data = frame[5:]
        try:
            csp_header = parse_csp(header)
        except:
            csp_header = ""
        print('\nMON:', frame)
        print('\tHeader: {},'.format(csp_header))
        print('\tData: {}'.format(data))
        print(prompt)

def console(port="8001", ip="localhost", origin=10):
    """ Send messages to node """
    ctx = zmq.Context(1)
    sock = ctx.socket(zmq.PUB)
    sock.connect('tcp://{}:{}'.format(ip, port))
    _prompt = "[node({}) port({})] <message>: "
    node = 1
    port = 10
    #          Prio   SRC   DST    DP   SP  RES HXRC
    header = "{:02b}{:05b}{:05b}{:06b}{:06b}00000000"

    while True:
        try:
            global prompt
            prompt = _prompt.format(node, port)
            cmd = input(prompt).split(" ")
            # case 1: <node> <port> <cmd> <arg1> ... <argn>
            # case 2: <cmd> <arg1> ... <argn>
            # case 3: <cmd>
            if len(cmd) > 2:
                try:
                    #case 1
                    node = int(cmd[0])
                    port = int(cmd[1])
                    data = " ".join(cmd[2:])
                except:
                    # case 2
                    data = " ".join(cmd)
            else:
                # case 2 or 3
                data = " ".join(cmd)

            if len(data) > 0:
                # Get CSP header and data
                hdr = header.format(1, int(origin), node, port, 63)
                # print("con:", hdr, hex(int(hdr, 2)))
                # print("con:", parse_csp(hex(int(hdr, 2))), data)

                # Build CSP message
                hdr_b = re.findall("........",hdr)[::-1]
                # print("con:", hdr_b, ["{:02x}".format(int(i, 2)) for i in hdr_b])
                hdr = bytearray([int(i,2) for i in hdr_b])
                msg = bytearray(node) + hdr + bytearray(data, "ascii")
                # print("con:", msg)
                sock.send(msg)
        except Exception as e:
            raise(e)

        time.sleep(0.25)

def get_parameters():
    """ Parse command line parameters """
    parser = argparse.ArgumentParser()

    parser.add_argument("-n", "--node", default=b'', help="Node address")
    parser.add_argument("-d", "--ip", default="localhost", help="Hub IP address")
    parser.add_argument("-i", "--in_port", default="8001", help="Input port")
    parser.add_argument("-o", "--out_port", default="8002", help="Output port")
    parser.add_argument("--nmon", action="store_false", help="Disable monitor task")
    parser.add_argument("--ncon", action="store_false", help="Disable console task")

    return parser.parse_args()

if __name__ == "__main__":
    # Get arguments
    args = get_parameters()
    print(args)

    tasks = []

    if args.nmon:
        # Start monitor thread
        mon_th = Thread(target=monitor, args=(args.out_port, args.ip, args.node))
        mon_th.daemon = True
        tasks.append(mon_th)
        mon_th.start()

    if args.ncon:
        # Create a console socket
        con_th = Thread(target=console, args=(args.in_port, args.ip, args.node))
        con_th.daemon = True
        tasks.append(con_th)
        con_th.start()

    for th in tasks:
        th.join()

