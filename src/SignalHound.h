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

#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <cmath>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "SHLAPI.h"

#define _ELPP_THREAD_SAFE 1
#define _ELPP_STL_LOGGING 1
#define _ELPP_BOOST_LOGGING 1
#define _ELPP_STACKTRACE_ON_CRASH 1
#define _ELPP_NO_DEFAULT_LOG_FILE 1
#include "../ext/easyloggingpp/src/easylogging++.h"

namespace SignalHound {
#define MAX_FREQ 4.4e9
#define MIN_FREQ 1.0

  typedef std::map<std::string, double> map_str_dbl;

  //External Trigger options
  enum triggerOpts {SHAPI_TRIGGERNORMAL = 0, SHAPI_SYNCOUT = 2, SHAPI_EXTERNALTRIGGER = 3};

  //Initial Configuration of the Signal Hound
  struct configOpts {
    double attenuation; /*!< Attenuation.  Values should be 0, 5, 10, or 15 */
    int mixerBand;  /*!< Set to 1 to work with freq above 150MHz.  0 (default) works below 150MHz */
    int sensitivity; /*!< Set the sensitivity 0=lowest, 2=highest */
    int decimation; /*!< Decimation level.  Affects RBW */
    int iflo_path; /*!< Set the IL LO path. 0 = use 10.7MHz IF LO, 1 = use 2.9MHz IF LO. */
    int adcclk_path; /*!< Select the ADC clock. 0 = ADC uses a 23-1/3MHz clock, 1 = ADC uses a 22.5MHz clock. */
    int deviceid; /*!< device ID, 0-7, seems to be disabled in the linux API */
    bool docal; /*!<if true, the following cal data will be used for calculations */
    unsigned char caldata[4096]; /*!< 4K cal data to use for calculations */
    bool preset;  /*!< attempt to preset the Signal hound after configuring */
    bool ext_ref; /*!< attempt to configure a 10MHz input clock */
    bool preamp; /*!< attempt to init preamp (SA44B only) */
    int ext_trigger; //set trigger to external, freerunning, or pulsed output */

    //Temperate compensation.  Not implemented as I dont have the API for it.
    bool dotemp; /*!< Not Implemented */
    char *temp_calfname; /*!< Not Implemented */

    configOpts() {
      attenuation = 10.0;
      mixerBand = 1;
      sensitivity = 0;
      decimation = 1;
      iflo_path = 0;
      adcclk_path = 0;
      deviceid = 0;
      docal = false;
      preset = false;
      ext_ref = false;
      preamp = false;
      ext_trigger = SHAPI_TRIGGERNORMAL;
      dotemp = false;
    }
  };

  struct rfOpts {
    bool slowSweep;  /*!< If true, performs slow sweep.  False performs a fast sweep */
    double start_freq; /*!< Sweep beginning frequency */
    double stop_freq; /*!< Sweep ending frequency */
    int fftsize; /*!< how large should the FFT window be */
    int image_rejection; /*!< 0 = mask both high and low, 1 = masks high side, 2 = masks low side */
    int average;  /*!< only used in slow sweep.  Arg is the number of ffts to average.  (fftsize*average) % 512 === 0 */

    rfOpts() {
      slowSweep = true;
      start_freq = 150.0e6;
      stop_freq = MAX_FREQ;
      fftsize = 1024;
      image_rejection = 0;
      average = 16;
    }
  };

  /** \brief Returns a string representation of the current date and time
   *
   * This low level function returns a string representation of the !LOCALTIME! 
   * time and date. include_ms will tack on milliseconds in '.%06d' format
   * you can also provide another format to pass to snprintf.  Using defaults
   * will return something like 2014-04-05 06:07:08.123456
   */
  void configureLoggers(bool ToFile=true, bool ToStandardOutput=true);
  std::string currentTimeDate(bool include_ms = true, const char* format = "%Y-%m-%d %H:%M:%S");

  class SignalHound {
    public:
      explicit SignalHound( struct configOpts *co = 0 /**< [in] Option set to acquire*/, struct rfOpts *rfo = 0 /** [in] RF option set to start with */);
      ~SignalHound();
      /** \brief Returns a human readable string of the current configuration options
       *
       * Returns a human readable string of the configuration options, as well
       * as a few calculated items like the Resolution Bandwith (RBW), sweep
       * count size, and Signal Hound Temperatures.
       *
       * @see SignalHound::SignalHound::temperature
       * @see SignalHound::SignalHound::calcRBW
       * @see SignalHound::SignalHound::sweepCount
       * @see SignalHound::SignalHound::sweepTime
       * @see SignalHound::SignalHound::frequencies
       * @see SignalHound::SignalHound::rfopts
       * @see SignalHound::SignalHound::configOpts
      */
      std::string info( void );
      map_str_dbl info_m( void );
      /** \brief Initialize the Signal Hound
       *
       * Returns 0 if success, and an error value otherwise.
       *
      */
      int  initialize( struct configOpts *co = 0 /**< [in] Option set to acquire */);

      /** \brief Array of Frequencies where measurements will occur.
       * 
       * Returns an vector of int frequencies where measurements should occur.
       */
      std::vector<int> frequencies( void );

      /** \brief Return Compensation Temperature of the Signal Hound*/
      double temperature( void );

      /** \brief Returns the Resolution Bandwidth*/
      double calcRBW( void ) { return ( 1.6384e6 / rfopts.fftsize / ( rfopts.slowSweep ? opts.decimation : 1 ) ); }

      /** \brief get/set the internal rfOpts structure;*/
      struct rfOpts rfOpts(struct rfOpts *n = 0);

      /** \brief Returns the number of samples in a sweep*/
      int sweepCount( void );

      /** \brief Returns the time (in seconds) a single sweep will take.*/
      double sweepTime(void );

      /** \brief Returns the frequency step size (in Hz)*/
      double sweepStep( void );

      /** \brief Perform a frequency sweep.
       *
       * Returns the size of the array returns, 0 if incorrectly
       * configured, and -sh_errno if the sweep failed.  Passing a
       * rfopts will use the specific configuration.
       *
       * @see SignalHound::SignalHound::rfopts
       * @see SignalHound::SignalHound::configOpts
       * @see SignalHound::SignalHound::calcRBW
       * @see SignalHound::SignalHound::sweepCount
       * @see SignalHound::SignalHound::sweepTime
       * @see SignalHound::SignalHound::frequencies
      */
      int sweep();
      int sweep( struct rfOpts rfopts /**< [in] RF Options to use */);

      /** \brief Returns True if the configuration settings are allowable
       *
       * Returns true if a sweep can be performed with the settings as currently
       * configured in SignalHound::SignalHound::rfopts. If it returns false, a
       * string error message will be placed in errmsg.  If true, errmsg will be
       * set to "No Errors".  Internally, the
       *
       * @see SignalHound::SignalHound::rfopts
       * @see SignalHound::SignalHound::configOpts
       * @see SignalHound::SignalHound::sweep
      */
      bool verfyRFConfig( std::string &errmsg /**< [out] String Error Message */, struct rfOpts ropts /**< [in] RF Options to use */);

      /**< \brief Measured values will be placed in here */
      std::vector<double> powers;

    private:
      struct rfOpts rfopts; //! Structure that contains all the pertinant information to run a sweep
      struct configOpts opts; //!Configuration Options for the Signal Hound
      int sh_errno; //!Error Number
      std::string errmsg; //! Error Message
      unsigned char *sighound_struct; //! Memory Block allocated for the SHAPI
      el::Logger* logger;
  };
}