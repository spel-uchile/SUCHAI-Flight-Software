import argparse
import socket
import re
from zmqnode import CspZmqNode, CspHeader, threaded


class DopplerNode(CspZmqNode):

    def __init__(self, this_node, radio_node, predict_ip="127.0.0.1", predict_port="4532", hub_ip="localhost", in_port="8001", out_port="8002", reader=False, writer=True):
        CspZmqNode.__init__(self, this_node, hub_ip, in_port, out_port, reader, writer)
        self.predict_ip = predict_ip
        self.predict_port = predict_port
        self.radio_node = radio_node
        self.f_main = 0
        self.f_sub = 0
        self._predict_th = None

    @threaded
    def predict_reader(self):
        re_main = re.compile(r"F +(\d+)\n")
        re_sub = re.compile(r"I +(\d+)\n")
        resp_ok = b"RPRT 0\n"
        csp_header = CspHeader(self.node, self.radio_node, 22, 10)

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind((self.predict_ip, int(self.predict_port)))
        s.listen()

        while self._run:
            print("Waiting predict connection...")
            conn, addr = s.accept()
            print("Predict connected to: {}".format(addr))

            while self._run:
                data = conn.recv(1024)

                if not data:
                    continue
                elif b'q\n' == data:
                    break
                elif b'F' in data:
                    self.f_main = re_main.findall(data.decode('ascii'))[0]
                    print("DN:", self.f_main)
                    conn.sendall(resp_ok)
                    self.send_message("com_set_config rx-freq {}".format(self.f_main), csp_header)
                elif b'I' in data:
                    self.f_sub = re_sub.findall(data.decode('ascii'))[0]
                    print("UP:", self.f_sub)
                    conn.sendall(resp_ok)
                    self.send_message("com_set_config tx-freq {}".format(self.f_sub), csp_header)
                elif b'f' in data:
                    resp = "{}\n".format(self.f_main).encode('ascii')
                    conn.sendall(resp)
                elif b'i' in data:
                    resp = "{}\n".format(self.f_sub).encode('ascii')
                    conn.sendall(resp)
                else:
                    print(data)
                    conn.sendall(resp_ok)

            conn.close()

        s.close()
        print("Predict stopped!")

    def start(self):
        CspZmqNode.start(self)
        self._predict_th = self.predict_reader()

    def join(self):
        CspZmqNode.join(self)
        self._predict_th.join()


def get_parameters():
    """ Parse command line parameters """
    parser = argparse.ArgumentParser()

    parser.add_argument("this_node", help="This node address")
    parser.add_argument("radio_node", help="Radio node address")
    parser.add_argument("--predict", default="localhost", help="Predict IP address")
    parser.add_argument("--pport", default="4532", help="Predict port")
    parser.add_argument("--hub", default="localhost", help="Hub IP address")
    parser.add_argument("-i", "--in_port", default="8001", help="Input port")
    parser.add_argument("-o", "--out_port", default="8002", help="Output port")

    return parser.parse_args()


if __name__ == "__main__":
    # Get arguments
    args = get_parameters()
    print(args)

    node = DopplerNode(int(args.this_node), int(args.radio_node), args.predict, args.pport, args.hub)
    node.start()
    try:
        node.join()
    except KeyboardInterrupt:
        node.stop()
