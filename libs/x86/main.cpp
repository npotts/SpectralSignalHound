// Author: Justin Crooks
// Test Equipment Plus
// Date: July 14, 2011

// Revision History:
// 7/14/11 Created.

//This is a simple C++ program to exercise the Signal Hound through the Linux API.

#include <iostream>
#include "CUSBSA.h"
using namespace std;

int main()
{
    CUSBSA mySignalHound;

    int i,returnCount = mySignalHound.FastSweep(310.0e6, 390.0e6);

    for(i=0; i<returnCount; i++)
        cout << mySignalHound.dTraceFreq[i]<< " Hz " << mySignalHound.dTraceAmpl[i] << " dBm"<< endl;


    return 0;
}

