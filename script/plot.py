import os

import matplotlib.pyplot as plt

def load_data(folderpath):
    # list all files in folderpath
    datas = []
    files = os.listdir(folderpath)
    files = sorted(files, key=lambda x: int(x.split(".")[0]))
    for file in files:
        # read line and convert to float array
        with open(folderpath + "/" + file, "r") as f:
            data = f.readlines()
            data = [float(x) for x in data]
            datas.append(data)

    return datas

def plot(datas):
    plt.figure()
    for data in datas:
        plt.plot(data)
    plt.show()

load_data("../trace/limit-rate")