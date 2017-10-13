#! /usr/bin/env python
#
# Processes the stats.csv file to generate graphs of the reponse times for each BPM

import csv
import numpy             as np
import matplotlib.pyplot as plt
import matplotlib.mlab   as mlab
from matplotlib import cm


with open("results.csv", "r") as file:
    results = list(csv.reader(file))

result_bins = {}

for i in range(1, len(results)):
    bpm   = str(results[i][1]).strip()
    delta = float(results[i][2])

    if(not bpm in result_bins):
        result_bins[bpm] = []
    result_bins[bpm].append(round(delta, 3))

for bpm in result_bins.keys():
    print("Outputting graph for: " + bpm + "bpm")

    plt.figure()
    plt.hist(x=result_bins[bpm], bins=21, range=(-0.3, 0.3))
    plt.title("Beat Delta for BPM: " + bpm)
    plt.xlabel("Beat Delta (seconds)")
    plt.ylabel("Count")
    plt.grid(True)
    plt.savefig("hist_" + bpm + ".svg")
    plt.close()
