#!/usr/bin/python

import csv
import time
from sys import stdout
from matplotlib.pylab import subplots, suptitle, savefig
import numpy as np
import matplotlib.dates as dates
import datetime

__all__= ["plotSpectrum"]

def datetime_from_string(t):
    """datetime from a string"""
    try:
        return datetime.datetime.strptime(t, "%Y-%m-%d %H:%M:%S.%f")
    except Exception as e:
        print("Cannot convert %s: %s" % (t, e))
        return None


def plotSpec(dat_filename):
    with open(dat_filename, "r") as csvfile:
        #read headers from the file
        headers = csvfile.readline().split(",")
        dreader = csv.DictReader(csvfile, fieldnames=headers, restkey=None, restval="-120")
        data={}
        for col in headers:
            data[col]=[]
        for row in dreader:
            for col in headers:
                try:
                    data[col].append(float(row[col]))
                except:
                    data[col].append(row[col])

    #data loaded
    np_time_grid = np.array([dates.date2num(datetime_from_string(t)) for t in data["timestamp"]])
    headers.remove("timestamp")
    headers.sort()
    plot_data=[]
    for col in headers:
        plot_data.append(data[col])
    plot_data = np.array(plot_data).astype(np.float)
    freq_headers = np.array(headers).astype(np.float)

    #plot the data
    fig, ax = subplots()
    pl = ax.pcolormesh(np_time_grid, freq_headers, plot_data, cmap='spectral', vmin=-100, vmax=-40)
    fig.colorbar(pl, orientation='vertical') #show a colorbar legend

    #add in title stuffs
    suptitle("Spectrum")
    ax.set_xlabel("Time")
    ax.set_ylabel("Frequency (Hz)")

    #setup time axis stuff

    locator = dates.MinuteLocator(30) #markers start 30 mins past the hour
    ax.xaxis.set_major_locator(locator)
    formatter = dates.AutoDateFormatter(locator)
    formatter.scaled[1/(24.*60.)] = '%H:%M:%S' # only show min and sec
    ax.xaxis.set_major_formatter(formatter)
    tmin = min(np_time_grid)
    tmax = max(np_time_grid)
    ax.set_xlim([tmin, tmax])
    fig.autofmt_xdate()

    #set image size and save it as a PNG
    fig.set_size_inches(18.5,10.5)

    #savefig("out.png", dpi=None, facecolor='w', edgecolor='w',
    #    orientation='portrait', papertype=None, format=None,
    #    transparent=False, bbox_inches=None, pad_inches=0.1,
    #    frameon=None)
    savefig("out.png", format="png", dpi=200, transparent=True, bbox_inches='tight')

plotSpec("metband.csv")