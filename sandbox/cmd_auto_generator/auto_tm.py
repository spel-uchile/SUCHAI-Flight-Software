import re
import os
import argparse


def file_to_text(directory):
    TXT = []
    for filename in os.listdir(directory):
        if filename=="repoDataSchema.h":
            location = directory + "/" + filename
            actual_file = open(location, 'r')
            return actual_file.read()
        else:
            continue
    return TXT

def get_functions(TXT):
    regexFile = r"(typedef struct __attribute__\(\(.?.?packed.?.?\)\) .{1,30} \{\n .*?(?:\})(.{1,30});)"  # txto + nombre de la tm
    # [0]=Function as a text
    # [1]=Name of the telemetry
    data = re.findall(regexFile, TXT, flags=re.DOTALL)
    return data

def get_function_data(function_list):
    regexFunction = r"(?:(char.?|uint16_t|uint32_t|int32_t|float|int) (.{1,30});(?:(?: *)\/\/\/< (.*)\n|\n))"  # sacar los valores dentro de cada tm
    # [0]=Data Type
    # [1]=Variable name
    # [2]=Variable Description
    function_data=[]

    for i in range(0,len(function_list)):
        aux = []
        data=re.findall(regexFunction,function_list[i][0])
        aux.extend(data)
        function_data.append(aux)
        i+=1
    return function_data

def get_name(function):
    final_name="TM_"
    name_info=(function.split("_"))
    switcher = {
        'data': 'PAYLOAD_',
        'status': 'STATUS',
        'entry' : 'ENTRY_'
    }
    if name_info[1]=='status':
        final_name=final_name+switcher.get(name_info[1])
    else:
        final_name = final_name+switcher.get(name_info[1])+name_info[0].upper()
    final_name=final_name.replace(" ","")
    return final_name


def get_tm(file_path,file_target):
    switcher = {
        'uint32_t': ' 32 UINT ',
        'int32_t': ' 32 INT ',
        'int16_t': ' 16 INT ',
        'float': ' 32 FLOAT ',
        'int': ' 32 INT ',
        'char': ' 320 STRING ',
        'char*': ' 320 STRING ',
    }
    file_as_string=file_to_text(file_path)
    function_list=get_functions(file_as_string)
    function_data=get_function_data(function_list)
    f = open(file_target,"w+")
    name_counter=0
    for i in range(0,len(function_list)):
        if("status" in get_name(function_list[i][1])):
            t_number=1
        else:
            t_number=10+name_counter
            name_counter+=1
        f.write("TELEMETRY SUCHAI_TARGET "+ get_name(function_list[i][1]) +" BIG_ENDIAN\n")
        f.write("""          APPEND_ITEM NODE 8 UINT "  "
          #APPEND_ITEM PRIORITY 2 UINT "Priority"
          #APPEND_ITEM SOURCE 5 UINT "Source"
          #APPEND_ITEM DEST 5 UINT "Destination"
          #APPEND_ITEM DEST_PORT 6 UINT "Destination port"
          #APPEND_ITEM SOURCE_PORT 6 UINT "Source port"
          #APPEND_ITEM RESERVED 4 UINT "Reserved"
          #APPEND_ITEM HMAC 1 UINT "HMAC"
          #APPEND_ITEM XTEA 1 UINT "XTEA"
          #APPEND_ITEM RDP 1 UINT "RDP"
          #APPEND_ITEM CRC 1 UINT "CRC"
          APPEND_ITEM HEADER 32 UINT "HEADER"
          APPEND_ITEM FRAME_NUMBER 16 UINT "Frame number\"""")

        f.write("\n")
        f.write("          APPEND_ID_ITEM TM_TYPE 16 UINT " +str(t_number)+ " \"Telemetry type\"")
        f.write("\n")

        for j in range(0,len(function_data[i])):
            current_data=function_data[i][j]
            aux_datatype=str(current_data[0])
            aux_variable_name=str(current_data[1])
            aux_variable_description=str(current_data[2])
            f.write("          APPEND_ITEM "+aux_variable_name.upper()+ switcher.get(aux_datatype)+"\""+aux_variable_description+"\"")
            f.write("\n")

            if "temp" in aux_variable_name:#add limits bar for the cosmos file
                f.write("""             UNITS CELSIUS C FORMAT_STRING "%0.3f"
                LIMITS DEFAULT 1 ENABLED -80.0 -70.0 60.0 80.0 -20.0 20.0
                LIMITS TVAC 1 ENABLED -80.0 -30.0 30.0 80.0""")
                f.write("\n")
            j += 1

        f.write("          APPEND_ITEM STRUCTS 0 BLOCK\n\n")
        f.write("\n")
        i+=1


if __name__ == "__main__":
    parser=argparse.ArgumentParser(description="Generate a COSMOS tlm.txt file")
    parser.add_argument('files_path',nargs='?',type=str,default="../../src/system/include")
    parser.add_argument('target_path',nargs='?',type=str,default="./suchai_tlm.txt")
    args=parser.parse_args()
    path1 = args.files_path
    path2=  args.target_path
    get_tm(path1, path2)

