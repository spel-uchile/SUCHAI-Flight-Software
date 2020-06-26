from subprocess import Popen, PIPE
from fuzzcspzmqnode import *
import os
import time
import sys
import json
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

    parser.add_argument('--exec_dir', type=str, default="../../build_x86/")
    parser.add_argument('--path_to_json', type=str, default="seqs/logfile-data-20200618-034256.txt")  # Assume we are on exec dir
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
    exec_cmd = "./SUCHAI_Flight_Software"
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

    # Read JSON report
    with open(path_to_json, 'r') as json_file:
        data = json.load(json_file)
        # Start sending sequence by id
        #for id in id_list:
        if len(data) != 0 and id_n < len(data):
            seq = data[id_n]
            cmds_params_list = seq["cmds"]
            cmds_l = []
            params_l = []
            for cmd_param in cmds_params_list:
                cmds_l.append(cmd_param["cmd_name"])
                params_l.append(cmd_param["params"])


    # Start sendind random commands
    for i in range(len(cmds_l)):
        # time.sleep(0.5)  # Give some time to zmqnode threads (writer and reader)
        cmd = cmds_l[i]
        params = params_l[i]
        print("node send:", cmd + " " + params)  # For debugging purposes
        header = CspHeader(src_node=int(addr), dst_node=int(dest), dst_port=int(port), src_port=55)
        node.send_message(cmd + " " + params, header)

    rm_end, vm_end = get_mem_info(proc_pid)

    # Exit SUCHAI process
    hdr = CspHeader(src_node=int(addr), dst_node=int(dest), dst_port=int(port), src_port=56)
    node.send_message("obc_reset", hdr)

    # Get SUCHAI process return code
    return_code = suchai_process.wait()
    end_time = time.time()

    if print_logfile:
        # Write log in file
        with open(log_path, 'wb') as logfile:
            for line in suchai_process.stdout:
                logfile.write(line)

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


def execute_seqs(exec_dir, path_to_json, log_path, protocol, print_logfile, id_list):
    subfolder_name = path_to_json.split('/')[-1].split('.')[0]
    for id_num in id_list:
        output_path = log_path + '/' + subfolder_name + '-' + str(id_num) + '.txt'
        execute_seq(exec_dir, path_to_json, output_path, protocol, print_logfile, id_num)


if __name__ == "__main__":
    args = get_parameters()
    execute_seqs(args.exec_dir, args.path_to_json, args.log_folder_path, args.protocol, args.save_log, args.id_list)