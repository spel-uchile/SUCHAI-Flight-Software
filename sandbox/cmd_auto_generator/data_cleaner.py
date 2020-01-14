import re
import os

def data_cleaner_cmd(TXT):
    """Return a list of tuples, that contains {function user name , function name, type of args, number of args}

    Keyword arguments:

    TXT -- a string that is all of the files already read to be processed

    """
    regex = r"    cmd_add\((.*),(.*),(.*),(.*)\);"
    DirtyData = []
    cleanData = []
    for x in TXT:
        ListOfCommands = re.findall(regex, x)
        DirtyData.extend(ListOfCommands)
    cleanData.extend(DirtyData)
    return cleanData


def data_cleaner_param(TXT):
    """Return a list of tuples, that contains {function name , name of the args}

    Keyword arguments:

    TXT -- a string that is all of the files already read to be processed

    """

    cleanData = []
    toRemove = []
    # why 47:len((char *fmt, char *params, int nparams))=37 + 10 = 47 its about the max length a function header will have
    regex= r"int (.{1,47}?)\((?:char..fmt, char..params, ?int nparams\))(?:\n| ?\{).*?(?:(?:sscanf\(params, fmt, (&.*?)\))|(?:\n\}))"
    DirtyData = []
    for x in TXT:
        ListOfCommands = re.findall(regex, x, flags=re.DOTALL)
        DirtyData.extend(ListOfCommands)
    cleanData.extend(DirtyData)

    for i in toRemove:
        cleanData.remove(i)

    return cleanData


def data_cleaner_dict(listCMD, listPARAM):
    """Return a list of tuples, that contains {function name , name of the args, type of arg, number of args}

        Keyword arguments:

        listCMD -- a list with all the information of a function
        listPARAM -- a list of the functions names with the name of the arguments

        """
    dict = {}
    toRemove = []
    whitelist = set('hjlLqtzdDiouxXfegEascpn')  # data types that could be arguments
    for i in range(0, len(listPARAM)):
        if listPARAM[i][1] != '':
            dict.update({listPARAM[i][0]: listPARAM[i][1]})
        else:
            pass
    for i in range(0, len(listCMD)):
        if listCMD[i][3] != 0:
            aux = list(listCMD[i])
            new_value = dict.get(aux[1])
            if new_value is not None:
                new_value = new_value.rstrip("\n")
                new_value = new_value.replace("\n", "")
                new_value = new_value.replace("&", "")
                new_value = new_value.replace(" ", "")
                new_value = new_value.split(",")
            aux[1] = new_value
            aux[2] = aux[2].replace("%", "")
            aux[2] = aux[2].replace("\"", "")
            aux[2] = aux[2].split(" ")
            toSlice = len(aux[0])
            aux[0] = aux[0][1:toSlice - 1]
            listCMD[i] = tuple(aux)
            if " " in listCMD[i][0]:
                toRemove.append(listCMD[i])
        else:
            pass

    for i in toRemove:
        listCMD.remove(i)
    return listCMD


def data_clean(files_path):
    """takes a directory and converts all the contents of it designed files (.c) to a list of tuples with all the data

                Keyword arguments:

                directory -- the path where this function will look for .c files

                """
    TXT=files_to_string(files_path)

    a = data_cleaner_cmd(TXT)
    b = data_cleaner_param(TXT)
    clean_data = data_cleaner_dict(a, b)
    return clean_data


def files_to_string(directory):
    """takes a directory and converts all the contents of it designed files (.c) to a single string

            Keyword arguments:

            directory -- the path where this function will look for .c files

            """
    TXT = []
    for filename in os.listdir(directory):
        if filename.endswith(".c"):
            location = directory + "/" + filename
            actualFile = open(location, 'r')
            TXT.append(actualFile.read())
            continue
        else:
            continue
    return TXT
