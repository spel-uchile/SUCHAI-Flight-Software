import zmq
import argparse
from threading import Thread

def parse_csp(hdrhex):
    hdrhex = int(hdrhex, 16)
    header = "S {}, D {}, Dp {}, Sp {}, Pr {}, HMAC {} XTEA {} RDP {} CRC32 {}".format(
        (hdrhex >> 30) & 0x03,
        (hdrhex >> 25) & 0x1f,
        (hdrhex >> 20) & 0x1f,
        (hdrhex >> 14) & 0x3f,
        (hdrhex >> 8) & 0x3f,
        "Yes" if ((hdrhex >> 3) & 0x01) else "No",
        "Yes" if ((hdrhex >> 2) & 0x01) else "No",
        "Yes" if ((hdrhex >> 1) & 0x01) else "No",
        "Yes" if ((hdrhex >> 0) & 0x01) else "No")

    return header

def monitor(port="8003"):
    """ Monitor socket running in background """
    ctx = zmq.Context(1)
    sock = ctx.socket(zmq.SUB)
    sock.setsockopt(zmq.SUBSCRIBE, b'')
    sock.connect('tcp://localhost:{}'.format(port))
    # Print all messages received in the socket
    while True:
        frame = sock.recv_multipart()[0]
        # header = ["{:08b}".format(i) for i in frame[1:5]]
        # print(header)
        header = ["{:08b}".format(i) for i in frame[1:5]][::-1]
        print(header)
        header_a = ["{:02x}".format(i) for i in frame[1:5]]
        # print(header)
        header = "0x"+"".join(header_a[::-1])
        data = frame[5:]
        try:
            csp_header = parse_csp(header)
        except:
            csp_header = ""
        print('MON:', frame)
        print('Header: {},'.format(csp_header), "Data: {}".format(data))

def console(port="8001"):
    import re
    ctx = zmq.Context(1)
    sock = ctx.socket(zmq.PUB)
    sock.connect('tcp://localhost:{}'.format(port))
    #          Prio  SRC   DST     DP     SP   RES HXRC
    header = "{:02b}{:05b}{:05b}{:06b}{:06b}00000000"
    while True:
        cmd = input("console >> ").split(" ", 1)
        dst, data = int(cmd[0]), cmd[1]
        hdr = header.format(1, 11, dst, 1, 1)
        #print(hdr, data)
        print(parse_csp(hex(int(hdr, 2))), data)
        hdr_b = re.findall("........",hdr)[::-1]
        print(hdr_b)
        hdr = "".join([chr(int(i,2)) for i in hdr_b])
        msg = chr(dst) + hdr + (data)
        msg = msg.encode("ascii", "replace")
        print(msg)
        sock.send(msg)

def get_parameters():
    """ Parse command line parameters """
    parser = argparse.ArgumentParser()

    parser.add_argument("-i", "--in_port", default="8001", help="Input port")
    parser.add_argument("-o", "--out_port", default="8002", help="Output port")
    parser.add_argument("-m", "--mon_port", default="8003", help="Monitor port")
    parser.add_argument("--mon", action="store_true", help="Enable monitor socket") 
    parser.add_argument("--con", action="store_true", help="Enable console task")

    return parser.parse_args()

if __name__ == "__main__":
    # Get arguments
    args = get_parameters()

    # Create sockets
    context = zmq.Context()
    xpub_out = context.socket(zmq.XPUB)
    xsub_in =  context.socket(zmq.XSUB)
    xpub_out.bind('tcp://*:{}'.format(args.out_port))
    xsub_in.bind('tcp://*:{}'.format(args.in_port))

    s_mon = None
    if args.mon:
        # Crate monitor socket
        s_mon = context.socket(zmq.PUB)
        s_mon.bind('tcp://*:{}'.format(args.mon_port))
        # Start monitor thread
        mon_th = Thread(target=monitor, args=(args.mon_port,))
        mon_th.start()

    # if args.con:
    #     # Create a console socket
    #     con_th = Thread(target=console, args=(args.in_port, ))
    #     con_th.start()

    # Start ZMQ proxy (blocking)
    zmq.proxy(xpub_out, xsub_in, s_mon)

