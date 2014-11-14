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

#include "SignalHoundCLI.h"
#define SVN_REV "0.0.0"

namespace po = boost::program_options;
namespace SignalHound {
  SignalHoundCLI::~SignalHoundCLI() {
    el::Loggers::unregisterLogger("SignalHoundCLI");
  }

  SignalHoundCLI::SignalHoundCLI(bool &ok, int argc, char *args[]): verbosity(NORMAL), sqlite(NULL), csv(NULL) {
    logger = getSignalHoundLogger("SignalHoundCLI");
    ok = parseArgs(argc, args);

    if (!ok) {
      CLOG(FATAL, "SignalHoundCLI") << "Something is wrong with the configuration.";
      return;
    }

    if ( dbfname != "" ) {
      //user specified some database filename
      CLOG(DEBUG, "SignalHoundCLI") << "Initializing Database: " << dbfname;
      sqlite = new SHBackendSQLite(ok, dbfname);
      CLOG_IF(!ok, ERROR, "SignalHoundCLI") << "Unable to open database.";
    } if (!ok) return;

    if ( csvfname != "" ) {
      CLOG(DEBUG, "SignalHoundCLI") << "Initializing CSV: " << csvfname;
      csv = new SHBackendCSV(ok, csvfname);
      CLOG_IF(!ok, ERROR, "SignalHoundCLI") << "Unable to open csv.";
    } if (!ok) return;

    //initialize the signal hound
    sighound.m_serialNumber = -1;
    int rtn = sighound.Initialize();
    CLOG(DEBUG, "SignalHoundCLI") << "Serial Number" << sighound.m_serialNumber << "Initialize() return:" << rtn;
    rtn = (sighound.m_serialNumber == -1) ? 1 : rtn; //HACK.  SOmething is wrong with the API 

    CLOG_IF( (rtn == 0), INFO,  "SignalHoundCLI") << "SignalHound Initialized";
    CLOG_IF( (rtn == 1), ERROR, "SignalHoundCLI") << "No SignalHound Found";
    CLOG_IF( (rtn == 2), ERROR, "SignalHoundCLI") << "Unable to open SignalHound";
    CLOG_IF( (rtn == 98), ERROR, "SignalHoundCLI") << "Unable to find temperature correction file D<serial-number>.bin";
    CLOG_IF( (rtn == 99), ERROR, "SignalHoundCLI") << "Unable to find calibration constants <serial-number>.tep";
    ok = (rtn == 0);
    if (rtn != 0) return;

    sighound.PreamplifierOnOff(preamp);
    if (extref) sighound.SetExternalRef();

    CLOG(DEBUG, "SignalHoundCLI") << "SignalHound Temperature" << sighound.ReadTemperature();
    sighound.SetCenterAndSpan(sighound.m_settings.m_centerFreq, sighound.m_settings.m_spanFreq);
    rtn = sighound.SetupForSweep();
    CLOG(DEBUG, "SignalHoundCLI") << "Sweep Parameters: " << ( rtn!=0  ? "Modified" : "Ok As-is");
    rtn = sighound.SetupForSweep();
    CLOG(DEBUG, "SignalHoundCLI") << "Sweep Parameters: " << ( rtn!=0  ? "Modified" : "Ok As-is");
    return;
  }
  void SignalHoundCLI::forceRange() {
    if ((sighound.m_settings.m_refLevel > 10) | (sighound.m_settings.m_refLevel > -150.0))
        sighound.m_settings.m_refLevel = -10;

      if ( (sighound.m_settings.m_attenIndex < 0) | (sighound.m_settings.m_attenIndex > 3))
        sighound.m_settings.m_attenIndex = 3;

      if (sighound.m_settings.m_RBWSetpoint == -1)
        sighound.m_settings.m_RBWIsAuto = true;
      else {
        sighound.m_settings.m_RBWIsAuto = false;
        if ( (sighound.m_settings.m_RBWSetpoint < 2) | (sighound.m_settings.m_RBWSetpoint > 24) ) {
          CLOG(INFO, "SignalHoundCLI") << "RBW set outside of allowable limits.  Forcibly setting auto-RBW";
          sighound.m_settings.m_RBWIsAuto = true;
        } else {
          sighound.m_settings.m_RBWIsAuto = false;
        }
      }

      if (sighound.m_settings.m_VBWSetpoint == -1)
        sighound.m_settings.m_VBWIsAuto = true;
      else {
        sighound.m_settings.m_VBWIsAuto = false;
        if ( (sighound.m_settings.m_VBWSetpoint < 2) | (sighound.m_settings.m_VBWSetpoint > 24) ) {
          CLOG(INFO, "SignalHoundCLI") << "VBW set outside of allowable limits.  Forcibly setting auto-VBW";
          sighound.m_settings.m_VBWIsAuto = true;
        } else {
          sighound.m_settings.m_VBWIsAuto = false;
        }
      }

      if ( (sighound.m_settings.m_SWPTMSetpoint < -1) | (sighound.m_settings.m_SWPTMSetpoint > 4))
        sighound.m_settings.m_SWPTMSetpoint = 0;

      switch(sighound.m_settings.m_VDMode) {
        case 1: sighound.m_settings.m_VDMode = HOUND_PROCESS_AS_POWER; break;
        case 2: sighound.m_settings.m_VDMode = HOUND_PROCESS_AS_VOLTAGE; break;
        case 3: sighound.m_settings.m_VDMode = HOUND_PROCESS_AS_LOG; break;
        default: sighound.m_settings.m_VDMode = HOUND_PROCESS_BYPASSED;
      }
      
      switch(sighound.m_settings.m_VDMMA) {
        case 1: sighound.m_settings.m_VDMMA = HOUND_VDMMA_MIN_MAX; break;
        case 2: sighound.m_settings.m_VDMMA = HOUND_VDMMA_MIN_ONLY; break;
        case 3: sighound.m_settings.m_VDMMA = HOUND_VDMMA_MAX_ONLY; break;
        default: sighound.m_settings.m_VDMMA = HOUND_VDMMA_AVERAGE_ONLY;
      }

      //make sure time is positive
      sighound.m_settings.m_ZSSweepTime = (sighound.m_settings.m_ZSSweepTime > 0) ? sighound.m_settings.m_ZSSweepTime : -sighound.m_settings.m_ZSSweepTime;    
  }
  bool SignalHoundCLI::parseArgs( int ac, char *av[]) {
    /* This ugly long function populates the two structures that configure the Signal Hound*/
    try {
      double start, stop, center, span;
      unsigned int ifbw;
      po::options_description od_general(
        ""
        "sh-spectrum: A Signal Hound (SA44B) Spectrum Analyzer Logger\n"
        "\nUsage: sh-spectrum <Arguments>\n"
        "General Options" );
      od_general.add_options()
      ( "help,h", "Show this message" )
      ( "version,V", "Print version information and quit" )
      ( "nostdout", "Nothing will be printed to stdout. Return value will indicate fatal errors. If you want to see errors, you can still use --log below." )
      ( "log,l", po::value<std::string>(&logfname)->default_value( "" ), "Write program log to file specified by arg." )
      ( "verbose,v", "Setting this will emit a gratuitous amount of babble." )
      ;
      po::options_description od_output( "Data Output" );
      od_output.add_options()
      ( "db", po::value<std::string>(&dbfname)->default_value( "" ), "Write data into a sqlite database specified by the arg." )
      ( "csv", po::value<std::string>(&csvfname)->default_value( "" ), "Produce a comma seperated file with data specified by arg." )
      ;
      po::options_description od_rfopts( "RF Options" );
      od_rfopts.add_options()
      ( "preamp", "If flag is set, will attempt to activate the built in RF preamplifier.  Only available on a Signal Hound SA44B." )
      ( "extref", "If flag is set, the Signal Hound will attempt to use a 10MHz external reference.  Input power to the Signal Hound must be greater than 0dBm in order for this to be used." )
      ( "center", po::value<double>(&center)->default_value( 403e6 ), "Set the center frequency in Hz.  Acceptable values are similar to 4e3, 4000, 4000.0 which all resolve to 4kHz." )
      ( "span", po::value<double>(&span)->default_value( 6e6 ), "Set the span in Hz.  This value is ignored in --zspan and --phase-noise modes." )
      ( "start", po::value<double>(&start)->default_value( -1 ), "Set the starting sweep frequency in Hz. If this and --stop are both not the defaults, these values will be used for the start and stop frequencies. This value is completely ignored in --zspan and --phase-noise modes, which rely on --center only." )
      ( "stop", po::value<double>(&stop)->default_value( -1 ), "Set the stop sweep frequency in Hz. If this and --start are both not the defaults, these values will be used for the start and stop frequencies. Simlar to --start, this value is completely ignored in --zspan and --phase-noise modes and strictly relies on --center only." )
      ( "reflevel", po::value<double>(&sighound.m_settings.m_refLevel)->default_value( -10.0 ), "Set the reference level in dBm.  Valid values are between +10.0 and -150.0")
      ( "attenuation", po::value<int>(&sighound.m_settings.m_attenIndex)->default_value( 3 ), "Set the input attenuation.\n0 = 0dB\n1 = 5dB\n2 = 10dB\n3 = 15dB")
      ( "rbw", po::value<int>(&sighound.m_settings.m_RBWSetpoint)->default_value( -1 ), "Attempt to set the resolution bandwidth (RBW).  The actual RBW will be altered to reflect what it physically possible. Default of -1 will automatically select a RBW. Allowed values are -1, and values between 2 and 24 (inclusive).\n2 = 5 MHz\n3 = 250 kHz\n4 = 100 kHz\n5 = 50 kHz\n6 = 25 kHz\n7 = 12.5 kHz\n8 = 6.4 kHz\n9 = 3.2 kHz\n10 = 1.6 kHz\n11 = 800 Hz\n12 = 400 Hz\n13 = 200 Hz\n14 = 100 Hz\n15 = 50 Hz\n16 = 25 Hz\n17 = 12.5 Hz\n18 = 6.4 Hz\n19 = 3.2 Hz\n20 = 1.6 Hz\n21 = .8 Hz\n22 = .4 Hz\n23 = .2 Hz\n24 = .1 Hz")
      ( "vbw", po::value<int>(&sighound.m_settings.m_VBWSetpoint)->default_value( -1 ), "Attempt to set the Video bandwidth (RBW).  The VBW needs to be smaller than the RBW, and is not used at all in Zero Span Sweep Mode. Default of -1 will automatically select a VBW. The values of VBW are the same as RBW.")
      ( "no-img-reject", "If set, it will remove the image rejection that is normally applied.")
      ( "sweep-speed", po::value<int>(&sighound.m_settings.m_SWPTMSetpoint)->default_value( 0 ), "Suggested Sweep speed. Allowed values are -1 to 4.  Use -1 sparingly, as documentation mentions it should rarely be set to -1.\n-1: Ub3r fast (b0rk3d?)\n0: (default) Fast\n...\n4: Slow.\nValues outside this range will fall back to the default." )
      ( "vdmode", po::value<int>(&sighound.m_settings.m_VDMode)->default_value( HOUND_PROCESS_BYPASSED ), "Set the mode for Video Detector Processing. Documentation is rather sparse, so the details here are gathered from varible names.\n1 = Process as power\n2 = Process as Voltage\n3 = Process as Log\n4 (default) = Bypass Video Processing." )
      ( "vdmma", po::value<int>(&sighound.m_settings.m_VDMMA)->default_value( HOUND_VDMMA_AVERAGE_ONLY ), "Set the mode for Video Detector Min Max. Documentation is rather sparse, so the details here are gathered from varible names.\n1 = Min-Max\n2 = Min Only\n3 = Max Only\n4 = Average Only." )
      ;
      po::options_description od_modes( "Operational Modes" );
      od_modes.add_options()
      ("delay", po::value<int>(&pause_between_traces)->default_value( 0 ), "In order to limit on the rediculously large file sizes, how long should this program pause between sweeps in milliseconds" )
      ( "sweeps", po::value<int>(&repetitions)->default_value( -1 ), "How many sweeps should be done before exiting.  Default of -1 means sweep forever (well... at least until Ctrl-C hit or power cycled)" )
      ( "slow", "Runs a slow sweep")
      ( "fast", "Runs a fast sweep")
      ( "rbw5MHz", "Sweep with a fixed RBW of 5MHz" )
      ( "zspan", "Run a Zero Span Sweep" )
      ( "phase-noise", "Run phase-noise measurement" )
      ( "tracking-gen", "Run a Tracking Generator sweep - Completely untested as I do not have a tracking generator signal hound." )
      ;
      po::options_description od_zerospan( "Zero Span Options" );
      od_zerospan.add_options()
      ( "zs-mode", po::value<int>(&sighound.m_settings.m_ZSMode)->default_value( 0 ), "Zero Span Demodulation Mode.  0=Amplitude, 1=Frequency, 2=Phase, all others will revert to default." )
      ( "zs-ifbw", po::value<unsigned int>(&ifbw)->default_value( 0 ), "Select the IF bandwidth. 1=240kHz, 2=120kHz, 4=60kHz, 8=30kHz, 16=15kHz.  Other values will fall back to the default." )
      ( "zs-sweep-time", po::value<double>(&sighound.m_settings.m_ZSSweepTime)->default_value( 1 ), "Set the sweep time (in seconds) when in zero span mode." )
      ;
      po::options_description od_phasenoise( "Phase Noise Options" );
      od_phasenoise.add_options()
      ( "pn-decade-start", po::value<int>(&sighound.m_settings.m_PNStartDecade)->default_value( 1 ), "Phase Noise Decade Start")
      ( "pn-decade-stop", po::value<int>(&sighound.m_settings.m_PNStopDecade)->default_value( 5 ), "Phase Noise Decade Stop")
      ;
      po::options_description all( "" );
      all.add( od_general ).add( od_output ).add( od_rfopts ).add( od_modes ).add( od_zerospan ).add( od_phasenoise );
      po::variables_map vm;
      po::store( po::command_line_parser( ac, av ).options( all ).run(), vm );
      po::notify( vm );
      if ( vm.count( "help" ) ) { std::cout << all << "\n"; exit( 0 );  }
      if ( vm.count("version") ) { std::cout << "sh-spectrum-logger rev-" << SVN_REV << std::endl; exit(0); }
      if ( vm.count("nostdout") )  tostdout = false;
      //Logger is already set to el::Level::Error, they want more...
      log_level = el::Level::Error;
      if ( vm.count("verbose") ) log_level = el::Level::Trace;
      
      getSignalHoundLogger(); //reconfigures output

      preamp = false;
      if ( vm.count("preamp") )
        preamp = true;

      extref = false;
      if ( vm.count("extref") )
        extref = true;

      ifbw = ((sighound.m_settings.m_sweepMode != HOUND_SWEEP_MODE_ZERO_SPAN) | (ifbw < 1) | (ifbw > 16) | ((ifbw & (~ifbw + 1) ) == ifbw) ) ? 0 : ifbw;
      switch(ifbw) {
        case 1:  sighound.m_settings.m_RBWSetpoint = HOUND_IFBW_240kHz; sighound.m_settings.m_VBWSetpoint = HOUND_IFBW_240kHz; break;
        case 2:  sighound.m_settings.m_RBWSetpoint = HOUND_IFBW_120kHz; sighound.m_settings.m_VBWSetpoint = HOUND_IFBW_120kHz; break;
        case 4:  sighound.m_settings.m_RBWSetpoint = HOUND_IFBW_60kHz;  sighound.m_settings.m_VBWSetpoint = HOUND_IFBW_60kHz; break;
        case 8:  sighound.m_settings.m_RBWSetpoint = HOUND_IFBW_30kHz;  sighound.m_settings.m_VBWSetpoint = HOUND_IFBW_30kHz; break;
        case 16: sighound.m_settings.m_RBWSetpoint = HOUND_IFBW_15kHz;  sighound.m_settings.m_VBWSetpoint = HOUND_IFBW_15kHz; break;
        default: ifbw = 0;
      }
      if (ifbw != 0) { //dont automatically set it to auto
        sighound.m_settings.m_RBWIsAuto = false;
        sighound.m_settings.m_VBWIsAuto = false;
      }

      sighound.m_settings.m_suppressImage = false;
      if (vm.count("no-img-reject"))
        sighound.m_settings.m_suppressImage = true;
      
      sighound.m_settings.m_sweepMode = -1;
      if (vm.count("slow")) sighound.m_settings.m_sweepMode=HOUND_SWEEP_MODE_SLOW_SWEEP;
      if (vm.count("fast")) sighound.m_settings.m_sweepMode=HOUND_SWEEP_MODE_FAST_SWEEP;
      if (vm.count("rbw5MHz")) sighound.m_settings.m_sweepMode=HOUND_SWEEP_MODE_RBW_5MHz;
      if (vm.count("zspan")) sighound.m_settings.m_sweepMode=HOUND_SWEEP_MODE_ZERO_SPAN;
      if (vm.count("phase-noise")) sighound.m_settings.m_sweepMode=HOUND_SWEEP_MODE_PHASE_NOISE;
      if (vm.count("tracking-gen")) sighound.m_settings.m_sweepMode=HOUND_SWEEP_MODE_TRACK_GEN;

      //since we know the mode, we can accurately apply center and 
      if ((sighound.m_settings.m_sweepMode == HOUND_SWEEP_MODE_ZERO_SPAN) | (sighound.m_settings.m_sweepMode == HOUND_SWEEP_MODE_PHASE_NOISE))
        span = 0.0; //only care about center freq, zero out span
      if ( (start > 1.0) && (stop > 1.0) && (stop > start) && (stop <= MAX_FREQ))
        sighound.SetStartAndStop(start, stop); //user specified start and stop values
      else
        sighound.SetCenterAndSpan(center, span); //default to using center and span values

      switch(sighound.m_settings.m_ZSMode) {
        case 1: sighound.m_settings.m_ZSMode = HOUND_ZS_MODE_AMPLITUDE; break;
        case 2: sighound.m_settings.m_ZSMode = HOUND_ZS_MODE_FREQUENCY; break;
        case 3: sighound.m_settings.m_ZSMode = HOUND_ZS_MODE_PHASE; break;
        default: sighound.m_settings.m_ZSMode = HOUND_ZS_MODE_AMPLITUDE;
      }

      forceRange();
      if (logfname != "" ) el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Filename, logfname);

      CLOG(DEBUG, "SignalHoundCLI") << "Freq Start " << sighound.m_settings.m_startFreq;
      CLOG(DEBUG, "SignalHoundCLI") << "Freq Stop " << sighound.m_settings.m_stopFreq;
      CLOG(DEBUG, "SignalHoundCLI") << "Freq Center " << sighound.m_settings.m_centerFreq;
      CLOG(DEBUG, "SignalHoundCLI") << "Freq Span " << sighound.m_settings.m_spanFreq;
      CLOG(DEBUG, "SignalHoundCLI") << "m_refLevel " << sighound.m_settings.m_refLevel;
      CLOG(DEBUG, "SignalHoundCLI") << "m_attenIndex " << sighound.m_settings.m_attenIndex;
      CLOG(DEBUG, "SignalHoundCLI") << "m_RBWIsAuto " << sighound.m_settings.m_RBWIsAuto;
      CLOG(DEBUG, "SignalHoundCLI") << "m_RBWSetpoint " << sighound.m_settings.m_RBWSetpoint;
      CLOG(DEBUG, "SignalHoundCLI") << "m_VBWIsAuto " << sighound.m_settings.m_VBWIsAuto;
      CLOG(DEBUG, "SignalHoundCLI") << "m_VBWSetpoint " << sighound.m_settings.m_VBWSetpoint;
      CLOG(DEBUG, "SignalHoundCLI") << "m_suppressImage" << sighound.m_settings.m_suppressImage;
      CLOG(DEBUG, "SignalHoundCLI") << "m_SWPTMSetpoint" << sighound.m_settings.m_SWPTMSetpoint;
      CLOG(DEBUG, "SignalHoundCLI") << "m_VDMode" << sighound.m_settings.m_VDMode;
      CLOG(DEBUG, "SignalHoundCLI") << "m_VDMMA" << sighound.m_settings.m_VDMMA;
      CLOG(DEBUG, "SignalHoundCLI") << "m_sweepMode " << sighound.m_settings.m_sweepMode;
      CLOG(DEBUG, "SignalHoundCLI") << "m_ZSMode " << sighound.m_settings.m_ZSMode;
      CLOG(DEBUG, "SignalHoundCLI") << "m_ZSSweepTime" << sighound.m_settings.m_ZSSweepTime;

      if ((dbfname != "") && (csvfname != "")) {
        std::cerr << "Please select a backend storage device.  Missing either --db or --csv" << std::endl;
        return false;
      }
      if (sighound.m_settings.m_sweepMode == -1) {
        std::cerr << "Please select a sweep method. Missing either --fast, --slow, --rbw5MHz, --zspan, or --phase-noise." << std::endl;
        return false;
      }

      try {
        sighound.m_settings.CalcSweepParams();
        return true;
      } catch (std::exception &e) {
        CLOG(INFO, "SignalHoundCLI") << "Unable to calculate sweep settings " << e.what();
        return false;
      }
    } catch ( std::exception &e ) {
      CLOG(FATAL, "SignalHoundCLI") <<  "Error parsing arguments: " << e.what();
      return false;
    }
    std::cout << "No Args Provided" << std::endl;
    return false;
  }
  bool SignalHoundCLI::runSweeps() {
    bool rtn = true;
    reps = 0;
    sighound.SetupForSweep();
    if (sqlite) sqlite->newSweep(sighound);
    if (csv) csv->newSweep(sighound);
    if (repetitions == -1) {
      CLOG(INFO, "SignalHoundCLI") << "Running until Ctrl-C is hit";
      while (1)
        runSweep();
    }
    if (repetitions > 0) {
      CLOG(INFO, "SignalHoundCLI") << "Running a total of " << repetitions << " sweeps";
      for (int i=0; i< repetitions; i++)
        rtn &= runSweep();
    } 
    return rtn;
  }

  bool SignalHoundCLI::runSweep() {
    CLOG(DEBUG, "SignalHoundCLI") << "Starting a Sweep";
    sighound.SetupForSweep();
    double temp = sighound.ReadTemperature();
    int rtn = sighound.DoSweep();
    CLOG_IF( (rtn != 0), DEBUG, "SignalHoundCLI") << "Error on sweep: " << rtn;
    if (rtn == 0) {
      std::vector<double> vals;
      vals.push_back(temp);
      int max_index = 0;
      double max = mW2dBm(sighound.pDataMax[0]);
      for (int i=0; i < sighound.m_traceSize; i++) {
        vals.push_back(mW2dBm(sighound.pDataMax[i]));
        if (mW2dBm(sighound.pDataMax[i]) > max) {
          max = mW2dBm(sighound.pDataMax[i]);
          max_index=i;
        }
      }
      std::cout << "\r" << "Sweeps Run# " << ++reps << " max Freq:" << sighound.GetFrequencyFromIdx(max_index) << " @ " << max << "dBm" << std::flush;
      CLOG(DEBUG, "SignalHoundCLI") << " Vals Size:" << vals.size();

      if (sqlite) {
        bool ok = sqlite->addSweep(vals);
        CLOG_IF(!ok, ERROR, "SignalHoundCLI") << "Data not inserted!";
      }
      if (csv) {
        bool ok = csv->addSweep(vals);
        CLOG_IF(!ok, ERROR, "SignalHoundCLI") << "Data not added to CSV!";
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(pause_between_traces));
    //usleep(pause_between_traces); //sleep between traces
    return (rtn == 0);
  }
};