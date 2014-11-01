/*

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
      database_fname="";
      sql_fname="";
      csv_fname="";
      log_fname="";
      caldata_fname="";

      freq_low=1e6;
      freq_high=1e7;
      attenuation=10.0;

      verbosity=1;
      mode=0;
      mixerBand=0;
      sensitivity=0;
      decimation=0;
      use29IF=0;
      ADCclock=0;

      fft=1024;

      preamp=false;
    }
  };

  bool parseOptions(int, char *[], struct cliopts &opts);
}
