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

#include "SignalHound.h"

//initialize easylogging++
_INITIALIZE_EASYLOGGINGPP

/* Log types available: (in hierarchical order):
LOG(INFO);
LOG(WARNING);
LOG(ERROR);
LOG(FATAL);
LOG(DEBUG);
LOG(TRACE);
*/

namespace SignalHound {
  std::string currentTimeDate(bool include_ms, const char* format)  {
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char buf[256];
    gettimeofday( &tv, NULL );
    nowtime = tv.tv_sec;
    nowtm = localtime( &nowtime );
    int len = strftime( buf, sizeof( buf ), format, nowtm );
    //fill in microseconds
    if (include_ms)
      len += snprintf( buf + len, sizeof( buf ) - len, ".%06d", ( unsigned int ) tv.tv_usec );
    std::string rtn = std::string( buf, len );
    return rtn;
  }

  bool tostdout = true; /// Initialize logger to default to stdout
  el::Level log_level = el::Level::Debug; ///Error messages should be Fatal and below.

  el::Logger* getSignalHoundLogger(std::string label) {
    el::Logger* logger = NULL;
    if (label != "") {
      logger = el::Loggers::getLogger(label);
      if (logger == NULL) {
        std::cout << "Logger " << label << " already exists" << std::endl;
        return NULL;
      }
    }
    //setup logging
    el::Loggers::addFlag(el::LoggingFlag::NewLineForContainer);
    el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);
    el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);
    el::Loggers::addFlag(el::LoggingFlag::LogDetailedCrashReason);
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::addFlag(el::LoggingFlag::CreateLoggerAutomatically);
    el::Loggers::addFlag(el::LoggingFlag::AutoSpacing); // LOG(DEBUG) << "a"<<"b"<<"c" prints as "a b c"
    //el::Loggers::addFlag(el::LoggingFlag::ImmediateFlush);
    //because the default timestamp is all wacky, we need to recreate a few logging format
    // INFO and WARNING are set to default by the global call below
    el::Loggers::reconfigureAllLoggers(                   el::ConfigurationType::Format, std::string("%datetime{%Y-%M-%d %H:%m:%s.%g} [%logger] %level %msg"));
    el::Loggers::reconfigureAllLoggers(el::Level::Error,  el::ConfigurationType::Format, std::string("%datetime{%Y-%M-%d %H:%m:%s.%g} [%logger] %level [%loc] %msg"));
    el::Loggers::reconfigureAllLoggers(el::Level::Fatal,  el::ConfigurationType::Format, std::string("%datetime{%Y-%M-%d %H:%m:%s.%g} [%logger] %level [%loc] %msg"));
    el::Loggers::reconfigureAllLoggers(el::Level::Debug,  el::ConfigurationType::Format, std::string("%datetime{%Y-%M-%d %H:%m:%s.%g} [%logger] %level [%loc] %msg"));
    el::Loggers::reconfigureAllLoggers(el::Level::Trace,  el::ConfigurationType::Format, std::string("%datetime{%Y-%M-%d %H:%m:%s.%g} [%logger] %level [%func] [%loc] %msg"));
    el::Loggers::reconfigureAllLoggers(el::Level::Verbose,el::ConfigurationType::Format, std::string("%datetime{%Y-%M-%d %H:%m:%s.%g} [%logger] %level-%vlevel [%logger] %msg"));
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToStandardOutput, tostdout ? "true": "false");
    el::Loggers::setLoggingLevel(log_level);
    return logger;
  }
  SignalHound::~SignalHound() {
    if ( sh_errno >= 0 ) {
      SHAPI_CyclePowerOnExit( sighound_struct );
      SHAPI_Close( sighound_struct );
    }
    free( sighound_struct );
    el::Loggers::unregisterLogger("SignalHound");
  }
  SignalHound::SignalHound( struct configOpts *co, struct rfOpts *rfo ) {
    //get a custom logger for this class to spew into
    logger = getSignalHoundLogger("SignalHound");
    //Initialize and create the signal hound
    if ( co )
      memcpy( &opts, co, sizeof( opts ) );
    if (rfo)
      memcpy( &rfopts, rfo, sizeof( rfopts ) );
    sh_errno = -1;
    int sh_size = SHAPI_GetStructSize();
    sighound_struct = ( unsigned char * ) malloc( sh_size * sizeof( double ) );
    //check malloc
    if ( sighound_struct == NULL )
      std::cerr << "malloc() failed?  The OOM killer is lurking nearby...." << std::endl;
    CLOG(DEBUG, "SignalHound") << "SignalHound object allocated and ready for Initialization";
  }
  int SignalHound::initialize( struct configOpts *co ) {
    if ( co )
      memcpy( &opts, co, sizeof( opts ) );
    //initialize.  Check if we can short cut with cal data
    if ( opts.docal ) { //yes, take the shortcut, but this does not seem to work any faster.  eh?
      CLOG(DEBUG, "SignalHound") << "Taking 'shorter' route with calibration data";
      sh_errno = SHAPI_InitializeEx( sighound_struct, opts.caldata );
    } else {
      CLOG(DEBUG, "SignalHound") << "Taking 'longer' route without calibration data";
      sh_errno = SHAPI_Initialize( sighound_struct );
      //copy cal data over now hat we have it
      if ( sh_errno == 0 ) {
        CLOG(DEBUG, "SignalHound") << "Copying cal data to internal array";
        SHAPI_CopyCalTable( sighound_struct, opts.caldata );
        opts.docal = true;
      }
    }
    CLOG_IF( (sh_errno != 0), ERROR, "SignalHound") << "Unable to Initialize Signal Hound";
    if ( sh_errno != 0 ) return sh_errno;

    CLOG_IF( (opts.preset), DEBUG, "SignalHound") << "Presetting Unit to Known State";
    if ( opts.preset ) { //Preset the unit
      if ( ( sh_errno = SHAPI_CyclePort( sighound_struct ) ) != 0 ) {
        CLOG_IF( sh_errno != 0, ERROR, "SignalHound") << "Unable to Preset the Signal Hound";
        return sh_errno;
      }
    }
    CLOG_IF( opts.preset, DEBUG, "SignalHound") << "Signal Hound Successfully Preset";

    //now configure
    if ( ( sh_errno = SHAPI_Configure( sighound_struct,
                                    opts.attenuation,
                                    opts.mixerBand,
                                    opts.sensitivity,
                                    opts.decimation,
                                    opts.iflo_path,
                                    opts.adcclk_path ) ) != 0 ) {
      CLOG_IF( opts.preset, ERROR, "SignalHound") << "Unable to Configure the Signal Hound.  errno =" << sh_errno;
      return sh_errno;
    }

    //Setup External Ref if asked
    if ( opts.ext_ref && ( ( sh_errno = SHAPI_SelectExt10MHz( sighound_struct ) ) != 0 ) )  {
      CLOG( ERROR, "SignalHound") << "Unable to set External Reference.  Does it have a large enough signal level?  errno =" << sh_errno;
      return sh_errno;
    }

    if ( opts.preamp && SHAPI_IsPreampAvailable(sighound_struct))
      SHAPI_SetPreamp( sighound_struct, 1 );
    else
      SHAPI_SetPreamp( sighound_struct, 0 );

    //rudimentary parameter validation
    if ( ( opts.ext_trigger != SHAPI_EXTERNALTRIGGER ) && ( opts.ext_trigger != SHAPI_SYNCOUT ) && ( opts.ext_trigger != SHAPI_TRIGGERNORMAL ) )
      opts.ext_trigger = SHAPI_TRIGGERNORMAL;

    SHAPI_SyncTriggerMode( sighound_struct, opts.ext_trigger );

    return sh_errno;
  }
  double SignalHound::temperature() {
    if ( !sh_errno )
      return ( double ) SHAPI_GetTemperature( sighound_struct );
    return ( -9999.9999 );
  }
  bool SignalHound::saveCalData( std::string fname_out) {
    if ( sh_errno ) return false;
    std::ofstream cal(fname_out.c_str(), std::ios::binary);
    if (cal.is_open()) {
      for(unsigned int i=0; i< sizeof(opts.caldata); i++) {
        cal << opts.caldata[i];
      }
    }
    cal.close();
    return true;
  }
  double SignalHound::sweepTime() {
    return ( rfopts.slowSweep ? \
             ( ( 40 + 1.0 * ( rfopts.fftsize * rfopts.average * opts.decimation ) / 486.0 ) * ( opts.decimation / 201.0e3 ) * ( rfopts.stop_freq - rfopts.start_freq ) ) : \
             ( 40 + 1.2 * ( rfopts.stop_freq - rfopts.start_freq ) / 200e3 ) ) / 1000.0;
  }
  int SignalHound::sweepCount() {
    return rfopts.slowSweep ? SHAPI_GetSlowSweepCount( sighound_struct, rfopts.start_freq, rfopts.stop_freq, rfopts.fftsize ) : \
                              SHAPI_GetFastSweepCount( rfopts.start_freq, rfopts.stop_freq, rfopts.fftsize );
  }
  double SignalHound::sweepStep( void ) {
    if ( rfopts.slowSweep ) {
      return 486111.111 / rfopts.fftsize / opts.decimation;
    } else { //fast sweep is a little different
      if ( rfopts.fftsize == 1 ) {
        return 200e3;
      } else {
        return (400.0e3 / rfopts.fftsize);
      }
    }
  }
  bool SignalHound::verfyRFConfig( std::string &errmsg, struct rfOpts ropts ) {
    //check if we are doing a slow or fast sweep
    bool ok = true;
    ok &= ( ropts.stop_freq > ropts.start_freq );
    if ( !ok ) {errmsg = "Stop Frequency must be higher than the Start Frequency"; CLOG(WARNING, "SignalHound") << errmsg; return false;}
    ok &= ( (ropts.stop_freq <= MAX_FREQ) && (ropts.start_freq >= MIN_FREQ) );
    if ( !ok ) {errmsg = "Allowable frequency range is from 1 Hz to 4.4GHz."; CLOG(WARNING, "SignalHound") << errmsg; return false;}
    if ( ropts.slowSweep ) {
      //check FFT length
      ok &= ( ( ropts.fftsize > 15 ) && ( ropts.fftsize <= 65536 ) && ( ( ropts.fftsize & ( ropts.fftsize - 1 ) ) == 0 ) );
      if ( !ok ) {errmsg = "FFT Size must be in the range of [16, 65536] and a power of 2."; CLOG(WARNING, "SignalHound") << errmsg; return false;}
      ok &= ( ( ropts.average * ropts.fftsize ) % 512 == 0 );
      if ( !ok ) {errmsg = "FFT Size * Average must be a multiple of 512"; CLOG(WARNING, "SignalHound") << errmsg; return false;}
    } else {
      ok &= ( ropts.fftsize == 1 ) | ( ( ropts.fftsize > 2 ) && ( ropts.fftsize <= 256 ) && ( ( ropts.fftsize & ( ropts.fftsize - 1 ) ) == 0 ) );
      if ( !ok ) {errmsg = "FFT Size must be either 1 for raw sample, or in the range of [16, 256] and a power of 2."; CLOG(WARNING, "SignalHound") << errmsg; return false;}
    }
    return ok;
  }
  int SignalHound::sweep( struct rfOpts o ) {
    rfopts = o;
    return sweep();
  }
  int SignalHound::sweep( void ) {
    if ( !verfyRFConfig( errmsg, rfopts ) ) return 0;
    //make sure the vectors are large enough
    int req_size = sweepCount(), sweep_return = 0;
    CLOG(DEBUG, "SignalHound") << "Sweeping with " << req_size << " data points";
    powers.reserve( req_size > 0 ? req_size : 0 ); //make sure we have enough slots to hold,  should be fast
    powers.resize( req_size > 0 ? req_size : 0 ); //this changes the size

    if ( rfopts.slowSweep ) {
      sweep_return =  SHAPI_GetSlowSweep( sighound_struct,
                                          powers.data(),
                                          rfopts.start_freq,
                                          rfopts.stop_freq,
                                          &req_size,
                                          rfopts.fftsize,
                                          rfopts.average,
                                          rfopts.image_rejection );
      if ( sweep_return != 0 ) return -sweep_return;
    } else {
      SHAPI_GetFastSweep( sighound_struct,
                          powers.data(),
                          rfopts.start_freq,
                          rfopts.stop_freq,
                          &req_size,
                          rfopts.fftsize,
                          rfopts.image_rejection );
      if ( sweep_return != 0 ) return -sweep_return;
    }
    return req_size;
  }
  std::vector<int> SignalHound::frequencies( void ) {
    //return a vector of frequencies based on the current configuration.
    std::vector<int> rtn;
    int req_size = sweepCount();
    if ( !verfyRFConfig( errmsg, rfopts ) ) return rtn;
    if ( rfopts.slowSweep ) {
      for ( int i = 0; i < req_size; i++ )
        rtn.push_back( ( int )( rfopts.start_freq + i * 486111.111 / rfopts.fftsize / opts.decimation ) );
    } else { //fast sweep is a little different
      if ( rfopts.fftsize == 1 ) {
        for ( int i = 0; i < req_size; i++ )
          rtn.push_back( rfopts.start_freq + i * 200e3 ); //spaced 200KHz apart
      } else {
        for ( int i = 0; i < req_size; i++ )
          rtn.push_back( rfopts.start_freq + i * 400.0e3 / rfopts.fftsize ); //spaced 400KHz/fftsize
      }
    }
    return rtn;
  }
  std::string SignalHound::info() {
    //create a formmatted string of status data
    std::stringstream rtn;
    rtn << "Frequency Start:                \t" << ( unsigned long ) rfopts.start_freq << " Hz" << std::endl;
    rtn << "Frequency Stop:                 \t" << ( unsigned long ) rfopts.stop_freq << " Hz" << std::endl;
    rtn << "Frequency Span:                 \t" << ( unsigned long )( rfopts.stop_freq - rfopts.start_freq ) << " Hz" << std::endl;
    rtn << "Center Frequency (Mean):        \t" << ( unsigned long )( rfopts.start_freq + ( rfopts.stop_freq - rfopts.start_freq ) / 2.0 ) << " Hz" << std::endl;
    rtn << "Center Frequency (Geometric):   \t" << ( unsigned long ) sqrt( rfopts.stop_freq * rfopts.start_freq ) << " Hz" << std::endl;
    rtn << "Resolution Bandwith:            \t" << calcRBW() << " Hz" << std::endl;
    rtn << "FFT Width:                      \t" << rfopts.fftsize << std::endl;
    rtn << "Image Rejection:                \t" << ( rfopts.image_rejection == 0 ? "On" : ( rfopts.image_rejection == 1 ? "High Side Only" : "Low Side Only" ) ) << std::endl;
    rtn << "Slow Sweep Average:             \t" << rfopts.average << std::endl;
    rtn << "Points per Sweep:               \t" << sweepCount() << std::endl;
    rtn << "Time per Sweep                  \t" << sweepTime() << " s" << std::endl;
    rtn << "Sweep Type:                     \t" << ( rfopts.slowSweep ? "Slow" : "Fast" ) << std::endl;
    rtn << "Attenuation (dBm):              \t" << ( int ) opts.attenuation << std::endl;
    rtn << "Sensitivity Level:              \t" << opts.sensitivity << std::endl;
    rtn << "Decimation:                     \t" << opts.decimation << std::endl;
    rtn << "IF LO:                          \t" << ( opts.iflo_path == 0 ? "10.7MHz" : "2.9MHz" ) << std::endl;
    rtn << "ADC Clock:                      \t" << ( opts.adcclk_path == 0 ? "23-1/3MHz" : "22.5MHz" ) << std::endl;
    rtn << "Device ID:                      \t" << opts.deviceid << std::endl;
    rtn << "Provided Calibration Data:      \t" << ( opts.docal ? "Yes" : "No" ) << std::endl;
    rtn << "Preset Signal Hound on Startup: \t" << ( opts.preset ? "Yes" : "No" ) << std::endl;
    rtn << "Attempt to Use External Reference:\t" << ( opts.ext_ref ? "Yes" : "No" ) << std::endl;
    rtn << "Attempt to use Preamp:          \t" << ( opts.preamp ? "Yes" : "No" ) << std::endl;
    rtn << "Signal Hound Temperature:       \t" << temperature() << std::endl;
    bool cfgok = verfyRFConfig( errmsg, rfopts );
    rtn << "Settings Valid for Sweep:       \t" << ( cfgok ? "Yes" : "No" ) << std::endl;
    if ( !cfgok )
      rtn << "Invalid Config Error Message: '" << errmsg << "'" << std::endl;
    return rtn.str();
  }
  map_str_dbl SignalHound::info_m() {
    //return a vstr of data from the
    map_str_dbl rtn;
    rtn["attenuation"] = (double) opts.attenuation;
    rtn["mixerband"] = (double) opts.mixerBand;
    rtn["sensitivity"] = (double) opts.sensitivity;
    rtn["decimation"] = (double) opts.decimation;
    rtn["iflo_path"] = (double) opts.iflo_path;
    rtn["adcclk_path"] = (double) opts.adcclk_path;
    rtn["deviceid"] = (double) opts.deviceid;
    rtn["docal"] = (double) opts.docal;
    rtn["preset"] = (double) opts.preset;
    rtn["ext_ref"] = (double) opts.ext_ref;
    rtn["preamp"] = (double) opts.preamp;
    rtn["ext_trigger"] = (double) opts.ext_trigger;
    rtn["slowsweep"] = (double) rfopts.slowSweep;
    rtn["start_freq"] = (double) rfopts.start_freq;
    rtn["stop_freq"] = (double) rfopts.stop_freq;
    rtn["span"] = (double) rfopts.stop_freq - rfopts.start_freq;
    rtn["center_mean"] = rfopts.start_freq + ( rfopts.stop_freq - rfopts.start_freq ) / 2.0;
    rtn["center_geometric"] = sqrt( rfopts.stop_freq * rfopts.start_freq );
    rtn["fftsize"] = (double)  rfopts.fftsize;
    rtn["image_rejection"] = (double) rfopts.image_rejection;
    rtn["average"] = (double) rfopts.average;
    rtn["valid"] = (double) verfyRFConfig( errmsg, rfopts );
    rtn["temperature"] = temperature();
    rtn["rbw"] = calcRBW();
    rtn["sweep_count"] = (double) sweepCount();
    rtn["sweep_time"] = sweepTime();
    rtn["sweep_step"] = sweepStep();
    return rtn;
  }
  struct rfOpts SignalHound::rfOpts(struct rfOpts *n) {
    if (n)
      rfopts = *n;
    return rfopts;
  }
};