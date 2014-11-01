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

#include "args.h"

namespace po = boost::program_options;
namespace SignalHound {


  bool parseOptions( int ac, char *av[], struct cliopts &opts ) {
    /* This ugly long function does one thing: create a structure
     * that continas the run config from a large set of specified
     * options */
    bool ext_ref;
    try {
      po::options_description od_general(
        ""
        "sh-spectrum-logger: A Signal Hound (SA44B) Spectrum Analyzer Logger\n"
        "\nUsage: SondeTool <Arguments>\n"
        "General Options" );
      od_general.add_options()
      ( "help,h", "Show this message" )
      ( "version,V", "Print version information and quit" )
      ( "quiet,q", "Setting this will cease all non-fatal displayed messages." )
      ( "verbose,v", "Setting this will cause a gratuitous amount of babble to be displayed.  This overrides --quiet." )
      ( "caldata,c",   po::value<std::string>()->default_value( "" ) , "Use this file as the calibration data for the signal hound.  This should radically spead up initialization.  Use 'sh-extract-cal-data' to extract this calibration data and reference it here." )
      ( "attenuation", po::value<double>()->default_value( 10.0 ), "Set the internal input attenuation.  Must be one of the following values: 0.0, 5.0, 10.0 (default), or 15.0.  Any other value will revert to the default." )
      ( "low-mixer",  "If flag is set, this will change the front end down converter to work with frequencies below 150MHz. If your frequency range will traverse above 150MHz, do not set this flag." )
      ( "sensitivity", po::value<int>()->default_value( 0 ), "Set the sensitivity of the Signal Hound.  0 (default) is lowest sensitivity, 2 is the highest." )
      ( "decimation",  po::value<int>()->default_value( 1 ), "Sample Rate is set to 486.111Ksps/arg.  Must be between [1, 16].  Resolution bandwidth is calculated from this and fft (below)." )
      ( "alt-iflo",    "If flag is set, this forces selection of the 2.9MHz Intermediate Frequency (IF) Local Oscillator (LO).  The default is 10.7MHz and has higher selectivity but lower sensitivity.  The 2.9MHz IF LO which features higher sensitivity yet lower selectivity." )
      ( "alt-clock",   "If flag is set, this forces selection of the 22.5MHz ADC clock.  The default uses a 23-1/3 MHz clock, but changing this is helpful if the signal you are interested in is a multiple of a 23-1/3MHz." )
      ( "device",      po::value<int>()->default_value( 0 ), "Select which Signal Hound Device to use.  Up to 8 can be connected to the same computer.  This seems to be disabled in the linux API" )
      ( "preset",      "If flag is set, the Signal Hound will be preset immediately after initialzing and prior to sampling.  This does set the Signal Hound to a known state, but it also adds another 2.5 seconds to start up time." )
      ( "ext",         po::value<bool>( &ext_ref )->default_value( false ), "If flag is set, the Signal Hound will attempt to use a 10MHz external reference.  Input power to the Signal Hound must be greater than 0dBm in order for this to be used." )
      ( "trigger",     po::value<int>()->default_value( 0 ), "Change the trigger mode.  0 (default) triggers immediately. 1  will only trigger on an external logic high. 2 will cause the trigger signal to pulse high when data collection begins." )
      ( "temp-cal",    po::value<std::string>()->default_value( "" ), "Filename of the temperate correction factors.  Often, it is a filename like \"D01234567.bin\".  Currently not implemented." )
      ( "preamp",      po::value<bool>()->default_value( false ), "If flag is set, will attempt to activate the built in RF preamplifier.  Only available on a Signal Hound SA44B." )
      ;
      po::options_description od_output( "Frequency Sweep data output options" );
      od_output.add_options()
      ( "database", po::value<std::string>()->default_value( "" ), "Write data into a sqlite database specified by the arg." )
      ( "sql,s", po::value<std::string>()->default_value( "" ), "Produce a text files that could be used to import data into a database." )
      ( "csv,c", po::value<std::string>()->default_value( "" ), "Produce a comma seperated file with data specified by arg." )
      ( "log,l", po::value<std::string>()->default_value( "" ), "Write program log to file specified by arg.  Defaults to stdout/stderr." )
      ;
      po::options_description od_modes( "Sweep Modes" );
      od_modes.add_options()
      ( "info", "Calculated parameters and dumps a list of what would be done.  This is helpful if you want to see the Resolution Bandwidth (RBW) or other RF parameters" )
      ( "slow", "Use slow sweep mode. Slow sweep is which is more thorough and not bandwidth limited.  Data points will be spaced 486.111KHz/(fft*decimation).  Each measurement cycle will take: (40 + (fft*average*decimation)/486)*(stop_freq - start_freq)/201000 milliseconds, rounded up. Furthermore, fft*average must be a integer multiple of 512." )
      ( "fast", "Use fast sleep mode. Fast sweep captures a single sweep of data. The start_freq, and stop_freq are rounded to the nearest 200KHz. If fft=1, only the raw power is sampled, and samples are spaced 200KHz apart. If fft > 1, samples are spaced 200KHz.  RBW is set solely on FFT size as the decimation is equal to 1 (fixed internally)" )
      ;
      po::options_description od_rfopts( "RF Options" );
      od_rfopts.add_options()
      ( "start", po::value<double>()->default_value( 1.0e6 ), "Lower bound frequency to use for the spectral sweep." )
      ( "stop", po::value<double>()->default_value( 1.0e7 ), "Upper bound frequency to use for the spectral sweep.  Due to rounding, you may get measured values past this value." )
      ( "image-rejection", po::value<int>()->default_value( 0 ), "Configure Image Rejection.  Default of 0 masks both high and low side injection.  Value of 1 only apply high side injection.  Value of 2 only applies low side injection." )
      ( "fft", po::value<int>()->default_value( -1 ), "Size of the FFT. Default value of -1 will autoselect the prefered FFT window. This and the decimation setting are used to calculate the RBW. In --slow mode, may be 16-65536 in powers of 2 while the default resolves to 1024.  In --fast mode, may be 1, 16-256 in powers of 2 while the default resolves to 16." )
      ( "average", po::value<int>()->default_value( 16 ), "Only used in --slow sweep.  Arg is the number of FFTs that get averaged together to produce the output. The value of (average*fft) must be an integer multiple of 512." )
      ;
      po::options_description all( "" );
      all.add( od_general ).add( od_output ).add( od_modes ).add( od_rfopts );
      boost::program_options::variables_map vm;
      po::store( po::command_line_parser( ac, av ).options( all ).run(), vm );
      po::notify( vm );
      if ( vm.count( "help" ) ) { std::cout << all << "\n"; exit( 0 );  }
      if ( vm.count( "ext" ) )
        std::cout << "ext" << vm["ext"].as<bool>() << endl;
      /*
      if ( vm.count("version") ) { std::cout << "SondeTool rev-" << SVN_REV << std::endl << std::endl << "This program makes use of SQLite (amalgamation) version: " << SQLITE_VERSION << std::endl << "\tSQLite Source ID: " << SQLITE_SOURCE_ID << std::endl << std::endl << "This program makes use of the MIT Licensed SQLiteCpp Library \r\nhttps://github.com/SRombauts/SQLiteCpp\r\nThose Portions are Copyright (c) 2012-2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)" << std::endl << "Version:" << SQLITECPP_VERSION << std::endl; exit(0); }

      if ( vm.count("quiet") ) rtn.verbosity = VERB_SILENT;
      if ( vm.count("verbose") ) rtn.verbosity = VERB_NOISY;
      if ( vm.count("dithering") ) rtn.dither = true;
      if ( vm.count("json") ) rtn.json = true;
      if ( vm.count("type") ) rtn.type = vm["type"].as<std::string>();
      for(unsigned int i=0; i< available_types.size(); i++) rtn.ok |= (available_types.at(i) == rtn.type); if (! rtn.ok) { std::cerr << "Unknown type selected: '" << rtn.type << "'. See --types for available types" << std::endl; rtn.ok = false; exit(EXIT_FAILURE); } rtn.ok = false;
      if ( vm.count("types") ) { std::cout << "Available decodable types:" << std::endl; for (unsigned int i=0; i < available_types.size(); i++) std::cout << "\t - " << available_types.at(i) << std::endl; std::cout << std::endl; exit(EXIT_SUCCESS); }
      if (vm.count("list")) { std::cout << "Parameters for format '" << rtn.type << "' :" << std::endl; std::vector<std::string> keys = lsd.getKeys(rtn.type); for(unsigned int i=0; i< keys.size(); i++) std::cout << "\t" <<  keys.at(i) << std::endl; exit(EXIT_SUCCESS); }
      if ( vm.count("precision") ) rtn.precision = vm["precision"].as<int>();
      if ( vm.count("parameters") ) { rtn.parameters.clear(); std::string params = vm["parameters"].as<std::string>(); while(params.find(',') != std::string::npos) { rtn.parameters.push_back(params.substr(0, params.find(','))); params = params.substr(params.find(',')+1); } rtn.parameters.push_back(params); }
      if ( vm.count("display") ) { rtn.cli_params.clear(); std::string params = vm["display"].as<std::string>(); while(params.find(',') != std::string::npos) { rtn.cli_params.push_back(params.substr(0, params.find(','))); params = params.substr(params.find(',')+1); } rtn.cli_params.push_back(params); }
      if ( vm.count("all") ) { rtn.all = true; if (rtn.type != "auto") rtn.parameters = lsd.getKeys(rtn.type); }
      if ( vm.count("serial-port") && (rtn.input_mode == MODE_BORKED)) { rtn.input_sport = vm["serial-port"].as<std::string>(); rtn.input_mode = MODE_SERIALCLIENT; }
      if ( vm.count("tcp-socket") && (rtn.input_mode == MODE_BORKED)) if (tcpformat_to_host_port(vm["tcp-socket"].as<std::string>(), rtn.input_tcp_host, rtn.input_tcp_port)) rtn.input_mode = MODE_TCPCLIENT;
      if ( vm.count("files") ) { rtn.output_batch_dir = vm["files"].as<std::string>(); if (vm.count("input") == 0) { std::cerr << "You must provide some files to process when in batch mode" << std::endl; exit(EXIT_FAILURE); } rtn.binary_files = vm["input"].as<std::vector<std::string> >(); rtn.input_mode = MODE_BATCH; }
      if ( vm.count("sql") ) { rtn.output_batch_dir = vm["sql"].as<std::string>(); std::cout << 1; if (vm.count("input") == 0) { std::cerr << "You must provide some files to process when in batch mode" << std::endl; exit(EXIT_FAILURE); } rtn.binary_files = vm["input"].as<std::vector<std::string> >(); rtn.input_mode = MODE_SQL; }
      if ( vm.count("flight") ) { rtn.output_batch_dir = vm["flight"].as<std::string>(); rtn.input_mode = MODE_FLIGHT;}
      if ( vm.count("avaps") ) { rtn.output_batch_dir = vm["avaps"].as<std::string>(); rtn.input_mode = MODE_AVAPSDATA;}
      if (vm.count("udp")) if (tcpformat_to_host_port(vm["udp"].as<std::string>(), rtn.output_udp_host, rtn.output_udp_port)) rtn.output_mode = rtn.output_mode | MODE_UDP;
      if (vm.count("bin")) { rtn.output_bin_fname = vm["bin"].as<std::string>(); rtn.output_mode = rtn.output_mode | MODE_BINFILE; }
      if (vm.count("decoded")) { rtn.output_decoded_fname = vm["decoded"].as<std::string>(); rtn.output_mode |= MODE_DECODEFILE;}
      if (rtn.input_mode == MODE_BORKED) {
        std::cerr << "Invalid Usage.  Use --help to see options" << std::endl;
        exit(EXIT_FAILURE);
      }
      rtn.ok = (rtn.input_mode != MODE_BORKED);
      */
      return true;
    } catch ( std::exception &e ) {
      std::cout << "Error parsing arguments: " << e.what() << std::endl;
      return false;
    }
    return false;
  }
};