/*
 * Copyright (c) 2014, Nick Potts
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of SpectralSignalHound nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

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
  len += snprintf( buf + len, sizeof( buf ) - len, ".%06d", ( unsigned int ) tv.tv_usec );
  string rtn = string( buf, len );
  return rtn;
}

int main( int args, char *argv[] ) {
  struct SignalHound::configOpts opt;
  opt.docal = true;
  memcpy( opt.caldata, caldata, sizeof( caldata ) );
  SignalHound::SignalHound sh( &opt );
  sh.rfopts.start_freq = 400e6;
  sh.rfopts.stop_freq = 401e6;
  //cout << sh.info();
  //cout << sh.measurement_frequencies() << endl;
  int init = sh.initialize();
  //cout << "Init = " << init  << endl;
  cout << cout << sh.info();
  cout << "Starting Sweep "  << endl;
  init = sh.sweep();
  cout << "Sweep Parameters = " << init  << endl;
  cout << "";
  for ( int i = 0; i < init; i++ ) {
    cout << i << " " << std::flush;
    cout << sh.powers.at( i );
  }

  vector<int> frq = sh.frequencies();
  for ( unsigned int i = 0; i < frq.size(); i++ ) {
    cout << frq[i] << " ";
  } cout << endl;






  struct SignalHound::cliopts opts;
  cout << parseOptions( args, argv, opts );
  exit( 0 );

  CUSBSA sighound;
  double f_low = 1.0e6,
         f_high = 3.0e7;
  int /*fft_len = 16,
      fft_len_slow = 1024,
      image_reject = 0, */
  count;

  //int err = sighound.Initialize( 0 );
  int err = sighound.Initialize( caldata );
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