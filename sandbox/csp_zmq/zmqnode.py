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


class CspHeader(object):
    def __init__(self, src_node=None, dst_node=None, src_port=None, dst_port=None, prio=2, hdr_bytes=None):
        """
        Represents a CSP header
        :param src_node: Int.
        :param dst_node: Int.
        :param src_port: Int.
        :param dst_port: Int.
        :param prio: Int.
        :param hdr_bytes: Bytes.
        """
        self.src_node = src_node
        self.dst_node = dst_node
        self.src_port = src_port
        self.dst_port = dst_port
        self.prio = prio
        self.hmac = False
        self.xtea = False
        self.rdp = False
        self.crc32 = False

        self.__bytes = None

        if hdr_bytes:
            self.from_bytes(hdr_bytes)

    def __str__(self):
        return "S {}, D {}, Dp {}, Sp {}, Pr {}, HMAC {} XTEA {} RDP {} CRC32 {}".format(
            self.src_node,
            self.dst_node,
            self.dst_port,
            self.src_port,
            self.prio,
            self.hmac,
            self.xtea,
            self.rdp,
            self.crc32)

    def __repr__(self):
        return self.__bytes.hex()

    def __int__(self):
        return int(self.__bytes.hex(), 16)

    def __hex__(self):
        return self.__bytes.hex()

    def __bytes__(self):
        return self.__bytes

    def from_bytes(self, hdr_bytes):
        """
        Parse header from byte array
        :param hdr_bytes: Array containing header bytes
        :return: None
        >>> hdr_bytes = bytes([0, 93, 160, 130])
        >>> hdr = CspHeader()
        >>> hdr.from_bytes(hdr_bytes)
        >>> hdr.dst_node
        10
        >>> hdr.dst_port
        1
        """
        assert len(hdr_bytes) == 4
        self.__bytes = hdr_bytes
        hdr_hex = bytes(reversed(hdr_bytes)).hex()
        hdr_int = int(hdr_hex, 16)
        self.__parse(hdr_int)

    def to_bytes(self):
        """
        Return the header as a byte array
        :return: Byte array
        >>> hdr_bytes = bytes([0, 93, 160, 130])
        >>> hdr = CspHeader(hdr_bytes=hdr_bytes)
        >>> hdr.to_bytes() == hdr_bytes
        True
        """
        self.__bytes = self.__dump()
        return self.__bytes

    def resend(self):
        """
        Swap node and port field to create a response header
        :return: None

        >>> hdr_bytes = bytes([0, 93, 160, 130])
        >>> hdr = CspHeader(src_node=1, dst_node=2, src_port=10, dst_port=20)
        >>> hdr.src_node
        1
        >>> hdr.resend()
        >>> hdr.src_node
        2
        >>> hdr.src_port
        20
        """
        dst_node = self.dst_node
        self.dst_node = self.src_node
        self.src_node = dst_node
        dst_port = self.dst_port
        self.dst_port = self.src_port
        self.src_port = dst_port

    def __parse(self, hdr_int):
        self.src_node = (hdr_int >> 25) & 0x1f
        self.dst_node = (hdr_int >> 20) & 0x1f
        self.dst_port = (hdr_int >> 14) & 0x3f
        self.src_port = (hdr_int >> 8) & 0x3f
        self.prio = (hdr_int >> 30) & 0x03
        self.hmac = True if ((hdr_int >> 3) & 0x01) else False
        self.xtea = True if ((hdr_int >> 2) & 0x01) else False
        self.rdp = True if ((hdr_int >> 1) & 0x01) else False
        self.crc32 = True if ((hdr_int >> 0) & 0x01) else False

    def __dump(self):
        #          Prio   SRC   DST    DP   SP  RES    H     X      R    C
        header = "{:02b}{:05b}{:05b}{:06b}{:06b}0000{:01b}{:01b}{:01b}{:01b}"
        hdr_bin = header.format(self.prio, self.src_node, self.dst_node, self.dst_port,
                                self.src_port, self.hmac, self.xtea, self.rdp, self.crc32)
        hdr_bin = re.findall("........", hdr_bin)[::-1]
        hdr_bytes = bytes([int(i, 2) for i in hdr_bin])
        return hdr_bytes


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

        >>> import time
        >>> node_1 = CspZmqNode(10)
        >>> node_1.read_message = lambda msg, hdr: print(msg, hdr)
        >>> node_1.start()
        >>> time.sleep(1)
        >>> node_1.stop()
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
        print("Reader started!")

        while self._run:
            # print("reading")
            try:
                frame = sock.recv_multipart()[0]
                # print(frame)
                header = frame[1:5]
                data = frame[5:]
                try:
                    csp_header = CspHeader()
                    csp_header.from_bytes(header)
                except:
                    csp_header = None

                if self.monitor:
                    print('\nMON:', frame)
                    print('\tHeader: {},'.format(csp_header))
                    print('\tData: {}'.format(data))

                self.read_message(data, csp_header)
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
        print("Writer started!")
        while self._run:
            try:
                # dnode, dport, sport, data = self._queue.get()
                data, csp_header = self._queue.get()
                print("W:", csp_header, data)
                if len(data) > 0:
                    # Get CSP header and data
                    hdr = csp_header.to_bytes()
                    msg = bytearray([int(csp_header.dst_node), ]) + hdr + bytearray(data, "ascii")
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

    def read_message(self, message, header=None):
        """
        Overwrite this method to process incoming messages. This function is automatically called by the reader thread
        when a new message arrives.

        :param message: Str. Message received
        :param header: CspHeader. CSP header
        :return:
        """
        raise NotImplementedError

    def send_message(self, message, header=None):
        """
        Call this function to send messages to another node. Destination node, port,
        and other options are contained in the header.
        This function automatically connects with the writer thread to send the messages.
        In general you do not need to overwrite this function, instead, you can simple use
        this function from your main thread.
        This function is thread safe because it uses a Queue to connect with the writer thread.

        :param message: Str. Message to send.
        :param header: CspHeader. CSP header object
        :return: None

        >>> node_1 = CspZmqNode(10, console=True)
        >>> node_1.start()
        >>> header = CspHeader(src_node=10, dst_node=11, dst_port=47, src_port=1)
        >>> node_1.send_message("hello_world", header)
        >>> node_1.stop()
        W: S 10, D 11, Dp 47, Sp 1, Pr 2, HMAC False XTEA False RDP False CRC32 False hello_world
        """
        self._queue.put((message, header))

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
        self.join()
        self._context.term()


def get_parameters():
    """ Parse command line parameters """
    parser = argparse.ArgumentParser()

    parser.add_argument("-n", "--node", default=9, help="Node address")
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
    node.read_message = lambda msg, hdr: print(msg, hdr)
    node.start()

    try:
        while True:
            dest, port, msg = input(prompt).split(" ", 2)
            hdr = CspHeader(src_node=int(args.node), dst_node=int(dest), dst_port=int(port), src_port=55)
            node.send_message(msg, hdr)
    except KeyboardInterrupt:
        node.stop()
