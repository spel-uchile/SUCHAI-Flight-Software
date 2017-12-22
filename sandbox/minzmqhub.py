import zmq
import argparse
from threading import Thread


def monitor(port="8003"):
    """ Monitor socket running in background """
    ctx = zmq.Context(1)
    sock = ctx.socket(zmq.SUB)
    sock.setsockopt(zmq.SUBSCRIBE, b'')
    sock.connect('tcp://localhost:{}'.format(port))
    # Print all messages received in the socket
    while True:
        print('MON:' , sock.recv_multipart())

def get_parameters():
    """ Parse command line parameters """
    parser = argparse.ArgumentParser()

    parser.add_argument("-i", "--in_port", default="8001", help="Input port")
    parser.add_argument("-o", "--out_port", default="8002", help="Output port")
    parser.add_argument("-m", "--mon_port", default="8003", help="Monitor port")
    parser.add_argument("--mon", action="store_true", help="Enable monitor socket") 

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
    
    # Start ZMQ proxy (blocking)
    zmq.proxy(xpub_out, xsub_in, s_mon)

