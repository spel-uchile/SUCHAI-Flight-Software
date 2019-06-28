import re
import zmq
import argparse
from threading import Thread
from queue import Queue


def threaded(fn):
    def wrapper(*args, **kwargs):
        thread = Thread(target=fn, args=args, kwargs=kwargs)
        thread.start()
        return thread
    return wrapper


class StopedException(Exception):
    pass


class CspZmqNode(object):

    def __init__(self, node, hub_ip='localhost', in_port="8001", out_port="8002", monitor=True, console=False):
        """
        CSP ZMQ NODE
        Is a PUB-SUB node connected to other nodes via the XSUB-XPUB hub
        NODE:PUB:OUT_PORT <----> HUB:XSUB:IN_PORT|::|HUB:XPUB:OUT_PORT <----> NODE:SUB:IN_PORT

        :param node: Int. This node address
        :param hub_ip: Str. Hub node IP address
        :param in_port: Str. Input port, SUB socket. (Should match hub output port, XPUB sockets)
        :param out_port: Str. Output port, PUB socket. (Should match hub input port, XSUB sockets)
        :param monitor: Bool. Activate reader.
        :param console: Bool. Activate writer.
        """
        self.node = int(node) if node else None
        self.hub_ip = hub_ip
        self.out_port = out_port
        self.in_port = in_port
        self.monitor = monitor
        self.console = console
        self._context = None
        self._queue = Queue()
        self._writer_th = None
        self._reader_th = None
        self._run = True

    @staticmethod
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

    @threaded
    def _reader(self, node=None, port="8001", ip="localhost", ctx=None):
        """
        Thread to read messages
        :param node: Int. Node to subscribe, usually self.node, use None to subscribe to all node messages.
        :param port: Str. Port to read message (SUB socket)
        :param ip: Str. Hub IP address, can be a remote node
        :param ctx: ZmqContext. Usually self._context or None to create a new context.
        :return: Thread.
        """
        _ctx = ctx if ctx is not None else zmq.Context(1)
        sock = _ctx.socket(zmq.SUB)
        sock.setsockopt(zmq.SUBSCRIBE, chr(int(node)).encode('ascii') if node is not None else b'')
        sock.setsockopt(zmq.RCVTIMEO, 1000)
        sock.connect('tcp://{}:{}'.format(ip, port))

        while self._run:
            # print("reading")
            try:
                frame = sock.recv_multipart()[0]
                # print(frame)
                header_a = ["{:02x}".format(i) for i in frame[1:5]]
                header = "0x"+"".join(header_a[::-1])
                data = frame[5:]
                try:
                    csp_header = self.parse_csp(header)
                except:
                    csp_header = ""

                if self.monitor:
                    print('\nMON:', frame)
                    print('\tHeader: {},'.format(csp_header))
                    print('\tData: {}'.format(data))

                self.read_message(data)
            except zmq.error.Again:
                pass

        sock.setsockopt(zmq.LINGER, 0)
        sock.close()
        if not ctx:
            _ctx.terminate()
        print("Reader stopped!")

    @threaded
    def _writer(self, origin, port="8002", ip="localhost", ctx=None):
        """
        Thread to send messages
        :param origin: Int. Node of origin, usually self.node.
        :param port: Str. Port to write messages (PUB socket)
        :param ip: Str. Hub IP address, can be a remote node
        :param ctx: ZmqContext. Usually self._context or None to create a new context.
        :return: Thread.
        """
        _ctx = ctx if ctx is not None else zmq.Context(1)
        sock = _ctx.socket(zmq.PUB)
        sock.connect('tcp://{}:{}'.format(ip, port))
        _prompt = "[node({}) port({})] <message>: "
        #          Prio   SRC   DST    DP   SP  RES HXRC
        header = "{:02b}{:05b}{:05b}{:06b}{:06b}00000000"
        origin = origin if origin is not None else 0

        while self._run:
            try:
                node, port, data = self._queue.get()

                if len(data) > 0:
                    # Get CSP header and data
                    hdr = header.format(1, int(origin), int(node), int(port), 63)
                    # print("con:", hdr, hex(int(hdr, 2)))
                    # print("con:", parse_csp(hex(int(hdr, 2))), data)

                    # Build CSP message
                    hdr_b = re.findall("........", hdr)[::-1]
                    # print("con:", hdr_b, ["{:02x}".format(int(i, 2)) for i in hdr_b])
                    hdr = bytearray([int(i, 2) for i in hdr_b])
                    msg = bytearray([int(node), ]) + hdr + bytearray(data, "ascii")
                    # print("con:", msg)
                    sock.send(msg)
            except Exception as e:
                print(e)
                break

        sock.setsockopt(zmq.LINGER, 0)
        sock.close()
        if not ctx:
            _ctx.terminate()
        print("Writer stopped!")

    def read_message(self, message):
        """
        Overwrite this method to process incoming messages. This function is automatically called by the reader thread
        when a new message arrives.

        :param message: Str. Message received
        :return:
        """
        raise NotImplementedError

    def send_message(self, message, node, port):
        """
        Call this function to send messages to certain node and port.
        This function automatically connects with the writer thread to send the messages.
        In general you do not need to overwrite this function, instead, you can simple use
        this function from your main thread.
        This function is thread safe because it uses a Queue to connect with the writer thread.

        :param message: Str. Message to send.
        :param node: Int. Destination node address
        :param port: Int. Destination node port
        :return: None
        """
        self._queue.put((node, port, message))

    def start(self):
        """
        Starts the node by starting the reader and writer threads (if correspond).
        If you override this function, do not forget to call the parent method,
        otherwise these thread are not initialized.
        :return: None
        """
        self._context = zmq.Context()
        if self.monitor:
            self._reader_th = self._reader(self.node, self.in_port, self.hub_ip, self._context)
        if self.console:
            self._writer_th = self._writer(self.node, self.out_port, self.hub_ip, self._context)

    def join(self):
        """
        This function joins the reader and writer threads. Can be used in the main thread to
        continue the node work in background.
        :return: None
        """
        if self.monitor:
            self._reader_th.join()
        if self.console:
            self._writer_th.join()

    def stop(self):
        self._run = False
        self._queue.put(("", "", ""))
        #raise StopedException("Stopped by user")
        self.join()
        self._context.term()


def get_parameters():
    """ Parse command line parameters """
    parser = argparse.ArgumentParser()

    parser.add_argument("-n", "--node", default=10, help="Node address")
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

    prompt = "<node> <port> <message>: "

    node = CspZmqNode(int(args.node), args.ip, args.in_port, args.out_port, args.nmon, args.ncon)
    node.start()

    print_msg = lambda msg: print(msg)
    node.read_message = print_msg

    try:
        while True:
            dest, port, msg = input(prompt).split(" ", 2)
            #print(dest, port, msg)
            node.send_message(msg, int(dest), int(port))
    except KeyboardInterrupt:
        node.stop()
