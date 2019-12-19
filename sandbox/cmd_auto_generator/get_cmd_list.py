import data_cleaner as DC
import argparse

def get_cmd_list(files_path,target_path):
    """takes a directory and converts all the contents of it designed files (.c) to a csv in the designated path

                Keyword arguments:

                files_path -- the path where this function will look for .c files
                target_path-- the path where the csv file will created of overwritten

                """
    data_list=DC.data_clean(files_path)
    f=open(target_path,"w+")
    f.write("command name, number of params, params%type--> \n")
    for x in data_list:
        f.write(x[0]+","+str(x[3]))
        if int(x[3])>0:
            for i in range (0, len(x[2])):
                if x[2][i]=='':
                    pass
                else:
                    if x[1] is None:#in case the var used is not from scanff
                        f.write(",")
                        f.write(" % " + x[2][i])
                    else:
                        f.write(",")
                        f.write("("+x[1][i]+")"+" % "+x[2][i])
        f.write("\n")



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Test 1")
    parser.add_argument('files_path', nargs='?', type=str,default="../../src/system")
    parser.add_argument('target_path', nargs='?', type=str, default="./suchai_cmd_list.csv")
    args = parser.parse_args()
    path1 = args.files_path
    path2 = args.target_path
    get_cmd_list(path1, path2)

