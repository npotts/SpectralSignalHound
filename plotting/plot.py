#!/usr/bin/python

# Copyright (c) 2014, Nick Potts
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
# 
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
# 
# * Neither the name of SpectralSignalHound nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


import csv
import time
import sqlite3
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

def dataFromSQLite(sqlite_fname, sql_table):
    """Returns a 2-tuple arrays of matplotlib time axis, and frequency data"""
    #PRAGMA table_info( your_table_name );
    with sqlite3.connect(sqlite_fname) as db:
        data={}

        cur = db.cursor();
        cur.execute("PRAGMA table_info( %s );" % sql_table) #get table structure
        headers = [row[1] for row in cur.fetchall()] 
        headers.remove("rowid")
        headers.remove("temperature")
        headers.sort();
        for col in headers:
            data[col]=[]
        q = "SELECT [%s] FROM %s" % ("],[".join(headers), sql_table) #form query to yank raw data
        cur.execute(q);
        for row in cur.fetchall():
            for i in range(len(headers)):
                data[headers[i]].append(row[i])
        timestamps = np.array([dates.date2num(datetime_from_string(t)) for t in data["timestamp"]])
        headers.remove("timestamp")
        plot_data=[data[col] for col in headers]
        plot_data = np.array(plot_data).astype(np.float)
        freq_headers = np.array(headers).astype(np.float)
        return (freq_headers, timestamps, plot_data);
    return (None, None, None)

def dataFromCSV(csv_fname):
    """Returns a 2-tuple arrays of matplotlib time axis, and frequency"""
    with open(csv_fname, "r") as csvfile:
        #read headers from the file
        headers = csvfile.readline().split(",")
        dreader = csv.DictReader(csvfile, fieldnames=headers, restkey=None, restval="-200")
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
        timestamps = np.array([dates.date2num(datetime_from_string(t)) for t in data["timestamp"]])
        headers.remove("timestamp")
        headers.remove("temperature")
        headers.sort()
        plot_data=[]
        for col in headers:
            plot_data.append(data[col])
        plot_data = np.array(plot_data).astype(np.float)
        freq_headers = np.array(headers).astype(np.float)
        return (freq_headers, timestamps, plot_data);

def plotSpec(dat_filename, table=""):
    out = dat_filename + ".png"
    timestamps = None
    plot_data = None
    freq_headers=None

    #hack until higher up function can specify plotting source
    if ".csv" in dat_filename or ".CSV" in dat_filename:
        (freq_headers, timestamps, plot_data) = dataFromCSV(dat_filename)
    if ".db" in dat_filename:
        (freq_headers, timestamps, plot_data) = dataFromSQLite(dat_filename, table)
        out = dat_filename + "_" + table + ".png"

    if timestamps == None or plot_data == None or freq_headers == None:
        print("No data to plot")
        return

    #plot the data
    fig, ax = subplots()
    pl = ax.pcolormesh(timestamps, freq_headers, plot_data, cmap='spectral', vmin=-100, vmax=-40)
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
    tmin = min(timestamps)
    tmax = max(timestamps)
    ax.set_xlim([tmin, tmax])
    fig.autofmt_xdate()

    #set image size and save it as a PNG
    fig.set_size_inches(18.5,10.5)

    #savefig("out.png", dpi=None, facecolor='w', edgecolor='w',
    #    orientation='portrait', papertype=None, format=None,
    #    transparent=False, bbox_inches=None, pad_inches=0.1,
    #    frameon=None)
    savefig(out, format="png", dpi=200, transparent=True, bbox_inches='tight')



#dataFromSQLite("403.db", "FAST_20141114L134906")
plotSpec("403.db", "FAST_20141114L134906")
#plotSpec("403-test-trace.csv")
#plotSpec("wide.csv")