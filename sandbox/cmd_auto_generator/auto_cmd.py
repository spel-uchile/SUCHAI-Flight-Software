import os
import data_cleaner as DC
import argparse

def write_CMD(list_item, target_path):
    """take a list of commands and writes them in the correct format for the COSMOS software to be able to read

            Keyword arguments:

            list_item -- a list with all the information of the commands of the suchai flight software
            target_path -- the path where the new file will be created or overwritten

            """
    switcher = {
        'd': '32 INT 0 2147483647 0',
        'u': '32 UINT 0 2147483647 0',
        'i': '32 INT 0 2147483647 0',
        'f': '32 INT 0 2147483647 0',
        's': '320 STRING',
        'p': '320 STRING',
        'n': '0 STRING',

    }
    f = open(target_path,"w+")
    f.write("""COMMAND SUCHAI_TARGET START BIG_ENDIAN "Starts something on the example target."
    #APPEND_ID_PARAMETER PACKET_ID 8 UINT 1 1 1 "Packet id"
    APPEND_PARAMETER LABEL 0 STRING "COSMOS" "The label to apply"
    """)
    f.write("\n")

    for x in list_item:
        lenName = len(x[0])
        if x[1] is None and int(x[3]) == 0:  # no param function
            f.write("COMMAND SUCHAI_TARGET " + x[0].upper() + " BIG_ENDIAN " + "\"description\"\n")
            f.write(
                "   APPEND_PARAMETER LABEL " + str(lenName * 8) + " STRING \"" + x[0] + "\" \"The label to apply\"\n")
        else:#the function has either params or vars scanned
            if x[1] is None and int(x[3]) > 0:  # params without scanf
                f.write("COMMAND SUCHAI_TARGET " + x[0].upper() + " BIG_ENDIAN " + "\"description\"\n")
                f.write("   APPEND_PARAMETER LABEL " + str(lenName * 8) + " STRING \"" + x[
                    0] + "\" \"The label to apply\"\n")
                n = 1
                while n <= int(x[3]):
                    f.write("   APPEND_PARAMETER SPACE_" + str(n) + " 8 STRING \" \"\n")
                    f.write("   APPEND_PARAMETER " + "VAR " + switcher.get(x[2][n - 1]) + " \"input\"\n")
                    n += 1

            elif x[1] is not None and int(x[3]) == 0:  # more vars scanned than params
                f.write("COMMAND SUCHAI_TARGET " + "WARNING:_" + x[0].upper() + " BIG_ENDIAN " + "\"description\"\n")
                f.write("   APPEND_PARAMETER LABEL " + str(lenName * 8) + " STRING \"" + x[
                    0] + "\" \"The label to apply\"\n")
                n = 1
                f.write("#  WARNING FUNCTION NOT WORKING AS INTENDED: more scanned vars than intended params \n")
                while n <= len(x[1]):
                    f.write("#   APPEND_PARAMETER SPACE_" + str(n) + " 8 STRING \" \"\n")
                    f.write("#   APPEND_PARAMETER " + '164 STRING ' + "\"input\"\n")
                    n += 1
                f.write("#  WARNING FUNCTION NOT WORKING AS INTENDED \n")
            else:  # normal function with scanned params
                f.write("COMMAND SUCHAI_TARGET " + x[0].upper() + " BIG_ENDIAN " + "\"description\"\n")
                f.write("   APPEND_PARAMETER LABEL " + str(lenName * 8) + " STRING \"" + x[
                    0] + "\" \"The label to apply\"\n")
                n = 1
                while n <= len(x[1]):
                    f.write("   APPEND_PARAMETER SPACE_" + str(n) + " 8 STRING \" \"\n")
                    f.write(
                        "   APPEND_PARAMETER " + x[1][n - 1].upper() + " " + switcher.get(x[2][n - 1]) + " \"input\"\n")
                    n += 1

        f.write("\n")
def create_cmd_cosmos(files_path, target_path):
    """take a path and from all the .c files in that path, it creates a new .txt file ready to be
     used by the COSMOS software with all the commands found

            Keyword arguments:

            files_path -- the path from where the function will look for .c files
            target_path -- the path where the new .txt will be created or overwritten

            """
    clean_data = DC.data_clean(files_path)
    write_CMD(clean_data, target_path)
    return 0


if __name__ == "__main__":
    parser=argparse.ArgumentParser(description="Generate a COSMOS cmd.txt file")
    parser.add_argument('files_path',nargs='?',type=str,default="../../src/system")
    parser.add_argument('target_path',nargs='?',type=str,default="./suchai_cmds.txt")
    args=parser.parse_args()
    path1 = args.files_path
    path2=  args.target_path
    create_cmd_cosmos(path1, path2)
