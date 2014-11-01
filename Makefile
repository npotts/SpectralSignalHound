INCLUDE_PATHS = -I. -I./src/ -I/opt/lib64
STATIC_LIBS=-lpthread -lboost_program_options -lftd2xx -ldl -lrt 
LDFLAGS=-L. -L./libs libs/sh_linux_api.a
CFLAGS= -Wall -O2


SOURCES=src/CUSBSA.cpp src/args.cpp src/SignalHound.cpp src/sh-spectrum.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sh-spectrum

CAL_SOURCES=src/CUSBSA.cpp src/extract-cal-data.cpp
CAL_OBJECTS=$(CAL_SOURCES:.cpp=.o)
CAL_EXECUTABLE=sh-extract-cal-data

all: $(SOURCES) $(CAL_SOURCES) $(EXECUTABLE) $(CAL_EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) -g -g3 $(INCLUDE_PATHS) -o $@ $(OBJECTS) $(LDFLAGS) $(STATIC_LIBS)

$(CAL_EXECUTABLE): $(CAL_OBJECTS)
	$(CXX) -g -g3 $(INCLUDE_PATHS) -o $@ $(CAL_OBJECTS) $(LDFLAGS) $(STATIC_LIBS)

.cpp.o:
	$(CXX) -g -g3 $(CFLAGS) $(INCLUDE_PATHS) $<  -c -o $@

clean:
	rm -f src/*.o *~ $(CAL_EXECUTABLE) $(EXECUTABLE) *.csv