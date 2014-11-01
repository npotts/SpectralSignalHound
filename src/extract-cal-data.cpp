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
#include "CUSBSA.h"

using namespace std;

int main( int argc, char *argv[] ) {
  CUSBSA sighound;
  cout << "Signal Hound Cal Data Extractor starting" << endl;
  int err = sighound.Initialize( 0 );
  cout << ( err ? "Unable to connect to Signal Hound" : "Connected to Signal Hound" ) << endl;
  if ( !err ) { //dump out cal table to file
    ofstream raw( "cal-data.h" );   raw << "unsigned char sh_cal_data[] = {";
    for ( unsigned int i = 0; i < sizeof( sighound.m_CalTable ); i++ )
      raw << int( sighound.m_CalTable[i] ) << ",";
    raw << "};" << endl;
    raw.close();
    cout << "raw cal data -> 'cal-data.h'" << endl;

    ofstream bin( "cal-data.cal", ios::binary );
    bin.write( ( char * ) sighound.m_CalTable, sizeof( sighound.m_CalTable ) );
    cout << "bin cal data -> 'cal-data.cal'" << endl;
    bin.close();
  }
}