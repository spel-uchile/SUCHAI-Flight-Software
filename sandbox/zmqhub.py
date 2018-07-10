import zmq
import argparse
from threading import Thread

prompt = "<origin> <dest> <port> <message>: "


def parse_csp(hdrhex):
    """
    Parse and print CSP header
    :param hdrhex: Str. Header as hexadecimal in a string
    :return: Str. Header parsed as string
    """
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
        # print("\nmon:", header)
        # header = ["{:08b}".format(i) for i in frame[1:5]][::-1]
        # print("mon:", header)
        header_a = ["{:02x}".format(i) for i in frame[1:5]]
        # print("mon:", header_a)
        header = "0x"+"".join(header_a[::-1])
        # print("mon:", header)
        data = frame[5:]
        try:
            csp_header = parse_csp(header)
        except:
            csp_header = ""

        # Print the header and message
        print('\nmon:', frame)
        print('\tHeader: {},'.format(csp_header))
        print("\tData: {}".format(data))
        print(prompt)

def console(port="8001"):
    """ Console socket to send CSP messages """
    import re
    ctx = zmq.Context(1)
    sock = ctx.socket(zmq.PUB)
    sock.connect('tcp://localhost:{}'.format(port))

    #          Prio   SRC   DST    DP   SP  RES HXRC
    header = "{:02b}{:05b}{:05b}{:06b}{:06b}00000000"

    while True:
        try:
            # Get CSP header and data
            cmd = input(prompt).split(" ")
            node, dst, port, data = int(cmd[0]), int(cmd[1]), int(cmd[2]), cmd[3]
            # print("con:", "{:05b}".format(node), "{:05b}".format(dst), "{:06b}".format(port), data)
            hdr = header.format(1, node, dst, port, 33)
            # print("con:", hdr, hex(int(hdr, 2)))
            # print(hdr, data)
            # print("con:", parse_csp(hex(int(hdr, 2))), data)

            # Build CSP message
            hdr_b = re.findall("........",hdr)[::-1]
            # print("con:", hdr_b, ["{:02x}".format(int(i, 2)) for i in hdr_b])
            hdr = bytearray([int(i,2) for i in hdr_b])
            msg = bytearray([dst,]) + hdr + bytearray(data, "ascii")
            # print("con:", msg)
            sock.send(msg)
        except Exception as e:
            print(e)

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
        mon_th.daemon = True
        mon_th.start()

    if args.con:
        # Create a console socket
        con_th = Thread(target=console, args=(args.in_port, ))
        con_th.daemon = True
        con_th.start()

    # Start ZMQ proxy (blocking)
    zmq.proxy(xpub_out, xsub_in, s_mon)

