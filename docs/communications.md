# Communications

## Setup
To test the communication system in GNU/Linux we need to setup two flight 
software nodes and the zmq node.

1. Go to sandbox/csp_zmq and execute the zmq node.
        
        python3 zmqhub.py --mon
    
    1.1 This will start the zmq node in the local host and ports 8001 and 8002
    
    1.2 Type ```python3 zmqhub.py --help``` to see more options
    
2. Modify the config.h to set the first node, then compile and execute the software

        #define SCH_COMM_ADDRESS        10  ///< Node address

3. Modify the config.h to set the second node, then compile and execute the software

        #define SCH_COMM_ADDRESS        11  ///< Node address
        
With this setup we have two CSP nodes that can talk each others

## Test

### Ping
Use one of the two running nodes and execute a ping from the console

        com_ping 11

Yo will se the following output, indicating the ping delay

        [INFO ][1526527450][Executer] Running the command: ping...
        [INFO ][1526527450][cmdCOM] Ping to 11 took 2
        [INFO ][1526527450][Executer] Command result: 1

### Send command
You can also remotely execute a command in other node using the ```com_send_cmd```
command

        com_send_cmd 11 help
        
In the sending node you will see the following output

        [INFO ][1526527608][Executer] Running the command: com_send_cmd...
        [INFO ][1526527608][Executer] Command result: 0
        
While in the receiving node the command was executed with the following output

        [INFO ][1526527608][Executer] Running the command: help...
        List of commands:
        Index    name    Params
        0        debug_obc       %d
        1        reset   
        2        get_mem         
        ...   
        13       ping    %d
        14       send_rpt        %d %s
        15       send_cmd        %d %n
        16       send_data       
        17       test    %s
        18       help    
        [INFO ][1526527608][Executer] Command result: 1

### Digital repeater

In the digital repeater mode the node A send a message to node B and then node
B broadcast the message to all available nodes. Use the ```com_send_rpt``` command
in A node

        com_send_rpt 11 hello-world!
        [INFO ][1526531381][Executer] Running the command: send_rpt...
        [INFO ][1526531381][Executer] Command result: 1
        
The output will show a received message in the RPT port of any connected node

        [INFO ][1526531381][Communications] RPT: hello-world!
        
 