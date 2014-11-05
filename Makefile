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

CC=gcc
CXX=g++
INCLUDE_PATHS = -I. -I./src/ -I/opt/lib64 -Iext/kompex/include -Iext/easyloggingpp/src
STATIC_LIBS=-lpthread -lboost_program_options -lftd2xx -ldl -lrt 
LDFLAGS=-L. -L./libs libs/sh_linux_api.a ext/kompex/lib/libkompex-sqlite-wrapper.a
CXXFLAGS=-g -g3 -Wall -O2 -std=c++11

SOURCES=src/SHBackend.cpp src/SHBackendCSV.cpp src/SHBackendSQLite.cpp src/SHWrapper.cpp src/SignalHound.cpp src/sh-spectrum.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sh-spectrum

#CAL_SOURCES=src/CUSBSA.cpp src/extract-cal-data.cpp
#CAL_OBJECTS=$(CAL_SOURCES:.cpp=.o)
#CAL_EXECUTABLE=sh-extract-cal-data

all: $(SOURCES) $(CAL_SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -o $@ $(OBJECTS) $(LDFLAGS) $(STATIC_LIBS)

#$(CAL_EXECUTABLE): $(CAL_OBJECTS)
#	$(CXX) -g -g3 $(INCLUDE_PATHS) -o $@ $(CAL_OBJECTS) $(LDFLAGS) $(STATIC_LIBS)

clean:
	rm -f src/*.o *~ $(CAL_EXECUTABLE) $(EXECUTABLE) *.csv *log