import re
import argparse
import pandas as pd

# General expressions
re_error = re.compile(r'\[ERROR\]\[(\d+)\]\[(\w+)\](.+)')
re_warning = re.compile(r'\[WARN \]\[(\d+)\]\[(\w+)\](.+)')
re_info = re.compile(r'\[INFO \]\[(\d+)\]\[(\w+)\](.+)')
re_debug = re.compile(r'\[DEBUG\]\[(\d+)\]\[(\w+)\](.+)')
re_verbose = re.compile(r'\[VERB \]\[(\d+)\]\[(\w+)\](.+)')

# Specific expressions
re_cmd_run = re.compile(r'\[INFO \]\[(\d+)]\[Executer\] Running the command: (.+)')
re_cmd_result = re.compile(r'\[INFO \]\[(\d+)]\[Executer\] Command result: (\d+)')


def get_parameters():
    """
    Parse script arguments
    """
    parser = argparse.ArgumentParser()
    # General expressions
    parser.add_argument('file', type=str, help="Log file")
    parser.add_argument('--error', action="store_const", const=re_error)
    parser.add_argument('--warning', action="store_const", const=re_warning)
    parser.add_argument('--info', action="store_const", const=re_info)
    parser.add_argument('--debug', action="store_const", const=re_debug)
    parser.add_argument('--verbose', action="store_const", const=re_verbose)
    # Specific expressions
    parser.add_argument('--cmd-run', action="store_const", const=re_cmd_run)
    parser.add_argument('--cmd-result', action="store_const", const=re_cmd_result)

    return parser.parse_args()


def parse_text(text, regexp):
    return regexp.findall(text)


def save_parsed(logs, file, format=None):
    df = pd.DataFrame(logs)
    # print(df)
    df.to_csv(file)


if __name__ == "__main__":
    args = get_parameters()

    print("Reading file {}...".format(args.file))
    with open(args.file) as logfile:
        text = logfile.read()

    args = vars(args)
    print(args)

    for type, regexp in args.items():
        if type is not "file" and regexp is not None:
            print("Parsing {}...", type)
            logs = parse_text(text, regexp)
            save_parsed(logs, args["file"]+type+".csv")


