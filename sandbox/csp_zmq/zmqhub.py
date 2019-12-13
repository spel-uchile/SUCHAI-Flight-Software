import re
import sys
import zmq
import argparse
from random import randint

sys.path.append("../csp_zmq")
print(sys.path)

from zmqnode import CspZmqNode
from zmqnode import threaded
from zmqnode import CspHeader

class CspZmqHub(CspZmqNode):

    def __init__(self, in_port="8002", out_port="8001", mon_port="8003", monitor=True, console=False):
        """
        CSP ZMQ HUB
        Is a PUB-SUB proxy that allow to interconnect a set of publisher and subscriber nodes.
        NODE:PUB:OUT_PORT <----> HUB:XSUB:IN_PORT|::|HUB:XPUB:OUT_PORT <----> NODE:SUB:IN_PORT

        :param in_port: input port, XSUB socket. (Should match nodes output port, PUB sockets)
        :param out_port: output port, XPUB socket. (Should match nodes input port, SUB sockets)
        :param mon_port: monitor port, internal PUB-SUB socket.
        :param monitor: activate monitor
        :param console: activate console
        """
        CspZmqNode.__init__(self, None, 'localhost', mon_port, in_port, monitor, console)
        self.mon_port_hub = mon_port
        self.out_port_hub = out_port
        self.in_port_hub = in_port

    def read_message(self, message, header=None):
        print(message)

    @threaded
    def console_hub(self):
        prompt = "[mac] <node> <port> <message>: "
        try:
            while self._run:
                #dest, port, msg = input(prompt).split(' ', 2)
                arguments = input(prompt)
                arguments = [a for a in re.split(r"(\d+) ", arguments) if a]
                if len(arguments) > 3:
                    mac, dest, port, msg = arguments[:]
                else:
                    dest, port, msg = arguments[:]
                    mac = dest
                print(dest, port, msg, mac)
                hdr = CspHeader(src_node=0, dst_node=int(dest), dst_port=int(port), src_port=randint(48, 63))
                hdr.mac_node = mac
                print(hdr, msg)
                self.send_message(msg, hdr)
        except Exception as e:
            print(e)
        print("Console stopped!")

    def start(self):
        # Start default writer and reader
        CspZmqNode.start(self)

        # Create sockets
        xpub_out = self._context.socket(zmq.XPUB)
        xsub_in = self._context.socket(zmq.XSUB)
        xpub_out.bind('tcp://*:{}'.format(self.out_port_hub))
        xsub_in.bind('tcp://*:{}'.format(self.in_port_hub))

        s_mon = None
        if self.monitor:
            # Crate monitor socket
            s_mon = self._context.socket(zmq.PUB)
            s_mon.bind('tcp://*:{}'.format(self.mon_port_hub))

        if self.console:
            self.console_hub()

        # Start ZMQ proxy (blocking)
        try:
            zmq.proxy(xsub_in, xpub_out, s_mon)

        except KeyboardInterrupt as e:
            print("Main:", e)

        finally:
            # print("Closing due to", e)
            xsub_in.setsockopt(zmq.LINGER, 0)
            xsub_in.close()
            xpub_out.close()
            if s_mon:
                s_mon.close()

            self.stop()


def get_parameters():
    """ Parse command line parameters """
    parser = argparse.ArgumentParser()

    parser.add_argument("-i", "--in_port", default="8002", help="Input port")
    parser.add_argument("-o", "--out_port", default="8001", help="Output port")
    parser.add_argument("-m", "--mon_port", default="8003", help="Monitor port")
    parser.add_argument("--mon", action="store_true", help="Enable monitor socket")
    parser.add_argument("--con", action="store_true", help="Enable console task")

    return parser.parse_args()


if __name__ == "__main__":
    # Get arguments
    args = get_parameters()
    print(args)
    zmqhub = CspZmqHub(args.in_port, args.out_port, args.mon_port, args.mon, args.con)
    zmqhub.start()



