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

#include "SHWrapper.h"
#define SVN_REV "0.0.0"

namespace po = boost::program_options;
namespace SignalHound {
  SHWrapper::~SHWrapper() {
    if (sh != NULL)
      delete(sh);
  }
  SHWrapper::SHWrapper(bool &ok, int argc, char *args[]): sh(NULL), verbosity(NORMAL), modes(SLOW_SWEEP) {
    std::string errmsg;
    ok = parseArgs(argc, args);
    sh = new SignalHound(&sh_opts, &sh_rfopts);
    std::cout << sh->info();
    ok &= sh->verfyRFConfig(errmsg, sh_rfopts);
  }

  void SHWrapper::message(std::string msg, int level) {
    if (level > verbosity)
      std::cout << msg << endl;
  }

  bool SHWrapper::parseArgs( int ac, char *av[]) {
    /* This ugly long function does one thing: populate the two structures
    that configure the Signal Hound*/
    try {
      po::options_description od_general(
        ""
        "sh-spectrum-logger: A Signal Hound (SA44B) Spectrum Analyzer Logger\n"
        "\nUsage: sh-spectrum-logger <Arguments>\n"
        "General Options" );
      od_general.add_options()
  /**/( "help,h", "Show this message" )
  /**/( "version,V", "Print version information and quit" )
  /**/( "quiet,q", "Setting this will cease all non-fatal displayed messages." )
  /**/( "verbose,v", "Setting this will cause a gratuitous amount of babble to be displayed.  This overrides --quiet." )
  /**/( "caldata,c", po::value<std::string>()->default_value( "" ) , "Use this file as the calibration data for the signal hound.  This should radically spead up initialization.  Use 'sh-extract-cal-data' to extract this calibration data and reference it here." )
  /**/( "attenuation", po::value<double>(&sh_opts.attenuation)->default_value( 10.0 ), "Set the internal input attenuation.  Must be one of the following values: 0.0, 5.0, 10.0 (default), or 15.0.  Any other value will revert to the default." )
  /**/( "low-mixer", "If flag is set, this will change the front end down converter to work with frequencies below 150MHz. If your frequency range will traverse above 150MHz, do not set this flag." )
  /**/( "sensitivity", po::value<int>(&sh_opts.sensitivity)->default_value( 0 ), "Set the sensitivity of the Signal Hound.  0 (default) is lowest sensitivity, 2 is the highest." )
  /**/( "decimation", po::value<int>(&sh_opts.decimation)->default_value( 1 ), "Sample Rate is set to 486.111Ksps/arg.  Must be between [1, 16].  Resolution bandwidth is calculated from this and fft (below)." )
  /**/( "alt-iflo", "If flag is set, this forces selection of the 2.9MHz Intermediate Frequency (IF) Local Oscillator (LO).  The default is 10.7MHz and has higher selectivity but lower sensitivity.  The 2.9MHz IF LO which features higher sensitivity yet lower selectivity." )
  /**/( "alt-clock", "If flag is set, this forces selection of the 22.5MHz ADC clock.  The default uses a 23-1/3 MHz clock, but changing this is helpful if the signal you are interested in is a multiple of a 23-1/3MHz." )
      //( "device", po::value<int>()->default_value( 0 ), "Select which Signal Hound Device to use.  Up to 8 can be connected to the same computer.  This seems to be disabled in the linux API" )
  /**/( "preset",  "If flag is set, the Signal Hound will be preset immediately after initialzing and prior to sampling.  This does set the Signal Hound to a known state, but it also adds another 2.5 seconds to start up time." )
  /**/( "extref", "If flag is set, the Signal Hound will attempt to use a 10MHz external reference.  Input power to the Signal Hound must be greater than 0dBm in order for this to be used." )
  /**/( "trigger", po::value<int>(&sh_opts.ext_trigger)->default_value( 0 ), "Change the trigger mode.  0 (default) triggers immediately. 1  will only trigger on an external logic high. 2 will cause the trigger signal to pulse high when data collection begins." )
      //( "temp-cal",    po::value<std::string>()->default_value( "" ), "Filename of the temperate correction factors.  Often, it is a filename like \"D01234567.bin\".  Currently not implemented." )
  /**/( "preamp",      "If flag is set, will attempt to activate the built in RF preamplifier.  Only available on a Signal Hound SA44B." )
      ;
      po::options_description od_output( "Data Output" );
      od_output.add_options()
      ( "database", po::value<std::string>()->default_value( "" ), "Write data into a sqlite database specified by the arg." )
      ( "sql,s", po::value<std::string>()->default_value( "" ), "Produce a text files that could be used to import data into a database." )
      ( "csv,c", po::value<std::string>()->default_value( "" ), "Produce a comma seperated file with data specified by arg." )
      ( "log,l", po::value<std::string>()->default_value( "" ), "Write program log to file specified by arg.  Defaults to stdout/stderr." )
      ;
      po::options_description od_rfopts( "RF Options" );
      od_rfopts.add_options()
  /**/( "start", po::value<double>(&sh_rfopts.start_freq)->default_value( 1.0e6 ), "Lower bound frequency to use for the spectral sweep." )
  /**/( "stop", po::value<double>(&sh_rfopts.stop_freq)->default_value( 1.0e7 ), "Upper bound frequency to use for the spectral sweep.  Due to rounding, you may get measured values past this value." )
  /**/( "image-rejection", po::value<int>(&sh_rfopts.image_rejection)->default_value( 0 ), "Configure Image Rejection.  Default of 0 masks both high and low side injection.  Value of 1 only apply high side injection.  Value of 2 only applies low side injection." )
  /**/( "fft", po::value<int>(&sh_rfopts.fftsize)->default_value( -1 ), "Size of the FFT. Default value of -1 will autoselect the prefered FFT window. This and the decimation setting are used to calculate the RBW. In --slow mode, may be 16-65536 in powers of 2 while the default resolves to 1024.  In --fast mode, may be 1, 16-256 in powers of 2 while the default resolves to 16." )
  /**/( "average", po::value<int>(&sh_rfopts.average)->default_value( 16 ), "Only used in --slow sweep.  Arg is the number of FFTs that get averaged together to produce the output. The value of (average*fft) must be an integer multiple of 512." )
      ;
      po::options_description od_modes( "Sweep Modes" );
      od_modes.add_options()
  /**/( "info", "Calculated parameters and dumps a list of what would be done.  This is helpful if you want to see the Resolution Bandwidth (RBW) or other RF parameters" )
  /**/( "slow", "Use slow sweep mode. Slow sweep is which is more thorough and not bandwidth limited.  Data points will be spaced 486.111KHz/(fft*decimation).  Each measurement cycle will take: (40 + (fft*average*decimation)/486)*(stop_freq - start_freq)/201000 milliseconds, rounded up. Furthermore, fft*average must be a integer multiple of 512." )
  /**/( "fast", "Use fast sleep mode. Fast sweep captures a single sweep of data. The start_freq, and stop_freq are rounded to the nearest 200KHz. If fft=1, only the raw power is sampled, and samples are spaced 200KHz apart. If fft > 1, samples are spaced 200KHz.  RBW is set solely on FFT size as the decimation is equal to 1 (fixed internally)" )
      ( "delay", po::value<int>(&pause_between_traces)->default_value( 0 ), "In order to limit on the rediculously large file sizes, how long should this program pause between sweeps in milliseconds" )
      ( "repetitions", po::value<int>(&repetitions)->default_value( -1 ), "How many sweeps should be done before exiting.  Default of -1 means sweep forever (well... at least until Ctrl-C hit or power cycled)" )
      ;
      po::options_description all( "" );
      all.add( od_general ).add( od_rfopts ).add( od_modes ).add( od_output );
      boost::program_options::variables_map vm;
      po::store( po::command_line_parser( ac, av ).options( all ).run(), vm );
      po::notify( vm );
      if ( vm.count( "help" ) ) { std::cout << all << "\n"; exit( 0 );  }
      if ( vm.count("version") ) { std::cout << "sh-spectrum-logger rev-" << SVN_REV << std::endl; exit(0); }
      if ( vm.count("quiet") ) verbosity = SILENT;
      if ( vm.count("verbose") ) verbosity = GRATUITOUS;
      if ( vm.count("low-mixer") ) sh_opts.mixerBand = 0;
      if ( vm.count("alt-iflo") ) sh_opts.iflo_path = 1;
      if ( vm.count("alt-clock") ) sh_opts.adcclk_path = 1;
      if ( vm.count("caldata") ) {
        //open the file and read in the bytes
        std::string fname = vm["caldata"].as<std::string>();
        try {
          std::ifstream bin(fname.c_str(), std::ios::binary);
          bin.read( (char *) sh_opts.caldata, 4096);
          if (bin.gcount() == 4096)
            sh_opts.docal = true;
        } catch (std::exception &e) {
          std::cerr << "Error opening cal file: " << e.what() << std::endl;
        }
      }
      if ( vm.count("preset") ) sh_opts.preset = true;
      if ( vm.count("extref") ) sh_opts.ext_ref = true;
      if ( vm.count("fast") ) sh_rfopts.slowSweep = false;
      if ( vm.count("info") ) modes = INFODISPLAY;
      if ( vm.count("slow") ) sh_rfopts.slowSweep = true;

      //perform validation
      if (!((sh_opts.attenuation == 0.0) | 
            (sh_opts.attenuation == 5.0) |
            (sh_opts.attenuation == 10.0) | 
            (sh_opts.attenuation == 15.0)))
        sh_opts.attenuation=10;
      if ( (sh_opts.sensitivity < 0) | (sh_opts.sensitivity > 2))
        sh_opts.sensitivity = 0;
      switch(sh_opts.ext_trigger) {
        case 0: sh_opts.ext_trigger = SHAPI_EXTERNALTRIGGER; break;
        case 1: sh_opts.ext_trigger = SHAPI_SYNCOUT; break;
        default: sh_opts.ext_trigger = SHAPI_TRIGGERNORMAL; break;
      }

      if ((sh_rfopts.image_rejection < 0) | (sh_rfopts.image_rejection > 2))
        sh_rfopts.image_rejection = 0;
      if (sh_rfopts.fftsize == -1)
        sh_rfopts.fftsize = sh_rfopts.slowSweep ? 1024 : 16;

      return true;
    } catch ( std::exception &e ) {
      std::cout << "Error parsing arguments: " << e.what() << std::endl;
      return false;
    }
    return false;
  }
};