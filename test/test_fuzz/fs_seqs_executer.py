from subprocess import Popen, PIPE
from fuzzcspzmqnode import *
import os
import time
import sys
import json
import glob

SCH_TRX_PORT_TM = 9               # ///< Telemetry port
SCH_TRX_PORT_TC = 10               # ///< Telecommands port
SCH_TRX_PORT_RPT = 11               # ///< Digirepeater port (resend packets)
SCH_TRX_PORT_CMD = 12               # ///< Commands port (execute console commands)
SCH_TRX_PORT_DBG = 13               # ///< Debug port, logs output


def get_parameters():
    """
    Parse script arguments
    """
    parser = argparse.ArgumentParser(prog='fs_seqs_executer.py')

    parser.add_argument('--exec_dir', type=str, default="build_test")
    parser.add_argument('--path_to_json', type=str, default="seqs")  # Assume we are on exec dir
    parser.add_argument('--log_folder_path', type=str, default="logs") # This folder must exist
    parser.add_argument('--protocol', type=str, default="tcp")
    parser.add_argument('--save_log', type=bool, default=True)
    parser.add_argument('--id_list', type=list, default=[0, 1])

    return parser.parse_args()


def execute_seq(exec_dir, path_to_json, log_path, protocol, print_logfile, id_n):

    # Run zmqhub.py
    ex_zmqhub = Popen(["python3", "../../sandbox/csp_zmq/zmqhub.py", "--ip", "127.0.0.1", "--proto", protocol], stdin=PIPE)

    # Send commands to the flight software through zmq
    dest = "1"
    addr = "9"
    port = str(SCH_TRX_PORT_CMD)
    node = FuzzCspZmqNode(addr, hub_ip="127.0.0.1", proto=protocol)
    node.start()

    # Execute flight software
    time.sleep(1)
    exec_cmd = "./SUCHAI_Flight_Software_Test"
    prev_dir = os.getcwd()
    # Run flight software sending n_cmds random commands with 1 random parameter
    os.chdir(exec_dir)
    if print_logfile:
        suchai_process = Popen([exec_cmd], stdin=PIPE, stdout=PIPE)
    else:
        suchai_process = Popen([exec_cmd], stdin=PIPE)

    time.sleep(4)
    os.chdir(prev_dir)

    # Clean database
    print("node send: drp_ebf 1010")  # For debugging purposes
    header = CspHeader(src_node=int(addr), dst_node=int(dest), dst_port=int(port), src_port=55)
    node.send_message("drp_ebf 1010", header)

    # Read JSON report
    with open(path_to_json, 'r') as json_file:
        seq_json = json.load(json_file)

    # Start sending sequences
    for seq in seq_json:
        # time.sleep(0.5)  # Give some time to zmqnode threads (writer and reader)
        for cmd in seq["cmds"]:
            print("node send:", cmd["cmd_name"] + " " + cmd["params"])  # For debugging purposes
            header = CspHeader(src_node=int(addr), dst_node=int(dest), dst_port=int(port), src_port=55)
            node.send_message(cmd["cmd_name"] + " " + cmd["params"], header)

    # Exit SUCHAI process
    hdr = CspHeader(src_node=int(addr), dst_node=int(dest), dst_port=int(port), src_port=56)
    node.send_message("obc_reset", hdr)

    # Get SUCHAI process return code
    return_code = suchai_process.wait()

    if print_logfile:
        # Write log in file
        with open(log_path, 'wb') as logfile:
            for line in suchai_process.stdout:
                logfile.write(line)
    print("Return code: ", return_code)  # For debugging purposes

    node.stop()

    os.chdir(prev_dir)

    # Kill zmqhub.py
    ex_zmqhub.kill()


def execute_seqs(exec_dir, path_to_json, log_path, protocol, print_logfile, id_list):
    seq_files = sorted(glob.glob(path_to_json+"/*.json"))
    for seq_file in seq_files:
        output_file = os.path.join(log_path, os.path.basename(seq_file)+".log")
        execute_seq(exec_dir, seq_file, output_file, protocol, print_logfile, None)


if __name__ == "__main__":
    args = get_parameters()
    execute_seqs(args.exec_dir, args.path_to_json, args.log_folder_path, args.protocol, args.save_log, args.id_list)