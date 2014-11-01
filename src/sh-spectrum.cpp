// Author: Justin Crooks
// Test Equipment Plus
// Date: July 14, 2011

// This is a simple C++ program to exercise the
//   Signal Hound through the Linux API.

#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fstream>
#include <cmath>
#include "signal-hound-cal-data.h"
#include "CUSBSA.h"
#include "args.h"
#include "SignalHound.h"

using namespace std;

std::string now() {
  struct timeval tv;
  time_t nowtime;
  struct tm *nowtm;
  char buf[64];
  gettimeofday( &tv, NULL );
  nowtime = tv.tv_sec;
  nowtm = localtime( &nowtime );
  int len = strftime( buf, sizeof( buf ), "%Y-%m-%d %H:%M:%S", nowtm );
  //fill in microseconds
  len += snprintf( buf + len, sizeof( buf ) - len, ".%06d", (unsigned int) tv.tv_usec );
  string rtn = string( buf, len );
  return rtn;
}

int main(int args, char *argv[]) {

  struct SignalHound::configOpts opt;
  SignalHound::SignalHound sh(&opt);
  cout << sh.info();
  sh.initialize();

  struct SignalHound::cliopts opts;
  cout << parseOptions(args, argv, opts);
  exit(0);

  CUSBSA sighound;
  double f_low = 1.0e6,
         f_high = 3.0e7;
  int /*fft_len = 16,
      fft_len_slow = 1024,
      image_reject = 0, */
      count;

  //int err = sighound.Initialize( 0 );
  int err = sighound.Initialize(caldata);
  cout << "Initializing: " << err << ( err ? " failed" : " connected" );

  //configure?
  sighound.Configure( 10, 0, 0, 1, 0, 0 );
  sighound.SetPreamp( 1 );
  /*sighound.Configure(double attenVal,
          int mixerBand,
          int sensitivity,
          int decimation,
          int useIF2_9,
          int ADCclock)
  */
  ofstream f( "metband.csv" );

  count = sighound.FastSweep( f_low, f_high );
  //count = sighound.SlowSweep(f_low, f_high, fft_len_slow);
  cout << now() << "> Count: " << count;

  f << "timestamp";
  for ( int i = 0; i < count; i++ )
    f << "," << ( sighound.trace_freq[i] == -INFINITY ? -120 : sighound.trace_freq[i] );
  f << endl << now();
  for ( int i = 0; i < count; i++ )
    f << "," << sighound.trace_ampl[i];
  f << endl;

  for ( int i = 0; i < 100; i++ ) {
    count = sighound.FastSweep( f_low, f_high );
    //count = sighound.SlowSweep(f_low, f_high, fft_len_slow);
    cout << now() << "> Count: " << count;
    f << now();
    for ( int i = 0; i < count; i++ )
      f << "," << ( sighound.trace_freq[i] == -INFINITY ? -120 : sighound.trace_freq[i] );
    f << endl;
  }
  return 0;
}