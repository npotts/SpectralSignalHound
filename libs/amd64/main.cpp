// Author: Justin Crooks
// Test Equipment Plus
// Date: July 14, 2011

// This is a simple C++ program to exercise the 
//   Signal Hound through the Linux API.

#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <fstream>

#include "CUSBSA.h"
using namespace std;

#define CHECK_STATUS(label, function)			\
	if(function) {								\
		cout << label << " failed\n";			\
	} else {									\
		cout << label << " success\n";			\
	}

double find_max(double *p, int len)
{
	double max = -1000.0;
	for(int i = 0; i < len; i++) {
		if(p[i] > max)
			max = p[i];
	}
	return max;
}

int main()
{
    CUSBSA mySignalHound;
    int count;
	
    cout << "Initializing\n";
    CHECK_STATUS("Initialize():", mySignalHound.Initialize(0));
	
    // Max Check
    for(int i = 0; i < 10; i++) {
		double max = 0.0;
		
		count = mySignalHound.FastSweep(310.0e6, 390.0e6);
		max = find_max(mySignalHound.trace_ampl, count);
		cout << "Sweep " << i << " max = " << max << endl;
    }
	
    cout << "Plotting sweep\n";
    count = mySignalHound.FastSweep(310.0e6, 390.0e6);
	
    ofstream file("trace.plot");
	
    for(int i = 0; i < count; i++)
		file << mySignalHound.trace_freq[i] << " "
			 << mySignalHound.trace_ampl[i] << endl;
	
    return 0;
}

