import urllib.request
import json
import re
import argparse
import pandas as pd

with urllib.request.urlopen("https://api.spel.cl/aprs_data") as url:
    data = json.loads(url.read().decode())

    data_frame = pd.DataFrame(data)
    print(data_frame)
    data_frame.to_csv('aprs_data.csv')
    # data_list = [list(h.values()) for h in data]
