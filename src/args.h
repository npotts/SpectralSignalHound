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


#pragma once
#include <boost/program_options.hpp>

using namespace std;
namespace SignalHound {

  struct cliopts {
    string database_fname;
    string sql_fname;
    string csv_fname;
    string log_fname;
    string caldata_fname;

    double freq_low;
    double freq_high;
    double attenuation;

    int verbosity;
    int mode;
    int mixerBand;
    int sensitivity;
    int decimation;
    int use29IF;
    int ADCclock;
    int fft;

    bool preamp;

    cliopts() {
      database_fname = "";
      sql_fname = "";
      csv_fname = "";
      log_fname = "";
      caldata_fname = "";

      freq_low = 1e6;
      freq_high = 1e7;
      attenuation = 10.0;

      verbosity = 1;
      mode = 0;
      mixerBand = 0;
      sensitivity = 0;
      decimation = 0;
      use29IF = 0;
      ADCclock = 0;

      fft = 1024;

      preamp = false;
    }
  };

  bool parseOptions( int, char *[], struct cliopts &opts );
}
