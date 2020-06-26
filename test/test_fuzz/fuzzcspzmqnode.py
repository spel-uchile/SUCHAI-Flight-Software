from sandbox.csp_zmq.zmqnode import *


class FuzzCspZmqNode(CspZmqNode):
    def __init__(self, node, hub_ip='localhost', in_port="8001", out_port="8002", reader=True, writer=True, proto="tcp"):
        """
        :param node:
        :param hub_ip:
        :param in_port:
        :param out_port:
        :param monitor:
        :param console:
        """
        CspZmqNode.__init__(self, node, hub_ip, in_port, out_port, reader, writer, proto)
        self.all_messages_queue = Queue()
        self.init_ready_queue = Queue()
        self.messages_list = []

    def read_message(self, message, header=None):
        """
        Put all received messages on a queue. This method is called from the _reader thread.
        :param message:
        :param header:
        :return:
        """
        """ This method is called from the _reader thread"""
        """print(message, header)
        if header.dst_port == 14:  
            self.init_ready_queue.put([message.decode('ASCII', 'ignore')])"""
        self.all_messages_queue.put([message.decode('ASCII', 'ignore')])

    def messages_queue_to_list(self):
        """
        Save in list all the messages from the messages queue.
        :return:
        """
        while not self.all_messages_queue.empty():
            self.messages_list.extend(self.all_messages_queue.get())

    def print_messages(self):
        """
        Print all the received messages as list.
        :return:
        """
        print("Full messages list:")
        print(self.messages_list)

    def filter_cmds_names(self):
        """
        Filter messages indicating that a command is being run and save its name.
        :return: Names of the command list.
        """
        if not self.messages_list:
            self.messages_queue_to_list()
        self.print_messages()
        cmds_names = [cmd.split()[3] for cmd in self.messages_list if 'Running the command' in cmd]
        print("Names of commands:")  # For debugging purposes
        print(cmds_names)  # For debugging purposes

        return cmds_names

    def filter_results(self):
        """
        Filter messages indicating a command result and save it.
        :return: List. Results of the commands sent.
        """
        if not self.messages_list:
            self.messages_queue_to_list()

        results = [result.split()[2] for result in self.messages_list if 'Command result' in result]
        print("Results")  # For debugging purposes
        print(results)  # For debugging purposes

        return results

    def filter_cmds_exec_time(self):
        """
        Filter messages indicating execution time of the commands and save it.
        :return: List. Commands time execution.
        """
        if not self.messages_list:
            self.messages_queue_to_list()

        cmds_time = [time.split()[-1] for time in self.messages_list if 'Command result' in time]
        print("Time")  # For debugging purposes
        print(cmds_time)  # For debugging purposes

        return cmds_time

    """def wait_init_ready(self):
        msg = self.init_ready_queue.get()
        print("WAIT INIT:", msg)"""
