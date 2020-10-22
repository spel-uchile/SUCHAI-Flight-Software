from subprocess import Popen, PIPE
from fuzzcspzmqnode import *
import os
import time
import sys
import json
import glob
from proc_info import *

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

    return parser.parse_args()


def execute_file_seqs(exec_dir, path_to_json, log_path, protocol, print_logfile):
    """
    Executes all sequences of a file.
    :param exec_dir: Execution directory. String.
    :param path_to_json: File path. String.
    :param log_path: Path where to save logs. String.
    :param protocol: Protocol used to send commands to the fs. String.
    :param print_logfile: Saves log file. Boolean.
    :return:
    """
    # Run zmqhub.py
    ex_zmqhub = Popen(["python3", "../../sandbox/csp_zmq/zmqhub.py", "--ip", "127.0.0.1", "--proto", protocol], stdin=PIPE)

    # Send commands to the flight software through zmq
    dest = "1"
    addr = "9"
    port = str(SCH_TRX_PORT_CMD)

    exec_cmd = "./SUCHAI_Flight_Software_Test"

    # Read JSON report
    with open(path_to_json, 'r') as json_file:
        seq_json = json.load(json_file)

    seq_num = 0
    for seq in seq_json:
        # Execute flight software
        node = FuzzCspZmqNode(addr, hub_ip="127.0.0.1", proto=protocol)
        node.start()
        time.sleep(1)
        prev_dir = os.getcwd()
        # Run flight software sending n_cmds random commands with 1 random parameter
        os.chdir(exec_dir)
        if print_logfile:
            init_time = time.time()  # Start measuring execution time of the sequence
            suchai_process = Popen([exec_cmd], stdin=PIPE, stdout=PIPE)
        else:
            init_time = time.time()  # Start measuring execution time of the sequence
            suchai_process = Popen([exec_cmd], stdin=PIPE)

        proc_pid = suchai_process.pid
        rm_start, vm_start = get_mem_info(proc_pid)

        time.sleep(4)
        os.chdir(prev_dir)

        # Clean database
        print("node send: drp_ebf 1010")  # For debugging purposes
        header = CspHeader(src_node=int(addr), dst_node=int(dest), dst_port=int(port), src_port=55)
        node.send_message("drp_ebf 1010", header)

        # Start sending sequences
        # time.sleep(0.5)  # Give some time to zmqnode threads (writer and reader)
        for cmd in seq["cmds"]:
            print("node send:", cmd["cmd_name"] + " " + cmd["params"])  # For debugging purposes
            header = CspHeader(src_node=int(addr), dst_node=int(dest), dst_port=int(port), src_port=55)
            node.send_message(cmd["cmd_name"] + " " + cmd["params"], header)

        rm_end, vm_end = get_mem_info(proc_pid)

        # Exit SUCHAI process
        hdr = CspHeader(src_node=int(addr), dst_node=int(dest), dst_port=int(port), src_port=56)
        node.send_message("obc_reset", hdr)

        # Get SUCHAI process return code
        return_code = suchai_process.wait()
        end_time = time.time()

        if print_logfile:
            # Write log in file
            output_file = os.path.join(log_path, str(seq_num) + "-" + os.path.basename(path_to_json).split(".")[0]+".log")
            with open(output_file, 'wb') as logfile:
                for line in suchai_process.stdout:
                    logfile.write(line)
        seq_num += 1

        print("Return code: ", return_code)
        print("Execution time (s): ", end_time - init_time)
        print("Memory usage (kb): ", rm_end - rm_start)

        #assert return_code == 0
        #assert end_time - init_time < 10
        #assert rm_end - rm_start

        node.stop()
        os.chdir(prev_dir)

    # Kill zmqhub.py
    ex_zmqhub.kill()


def execute_files_seqs(exec_dir, path_to_json, log_path, protocol, print_logfile):
    seq_files = sorted(glob.glob(path_to_json+"/*.json"))
    for seq_file in seq_files:
        #output_file = os.path.join(log_path, os.path.basename(seq_file)+".log")
        execute_file_seqs(exec_dir, seq_file, log_path, protocol, print_logfile)


if __name__ == "__main__":
    args = get_parameters()
    execute_files_seqs(args.exec_dir, args.path_to_json, args.log_folder_path, args.protocol, args.save_log)
