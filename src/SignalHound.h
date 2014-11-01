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

#include <cmath>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "SHLAPI.h"

namespace SignalHound {
#define MAX_FREQ 4.4e9
#define MIX_FREQ 1.0
  enum connectionStates {UNCONNECTED = 0, INITIALIZED, PRESET, CONFIGURED};

  //External Trigger options
  enum triggerOpts {SHAPI_EXTERNALTRIGGER = 0, SHAPI_SYNCOUT = 2, SHAPI_TRIGGERNORMAL = 3};

  //Initial Configuration of the Signal Hound
  struct configOpts {
    double attenuation;
    int mixerBand;  //set to 1 to work with freq below 150MHz.  0 (default) works above 150MHz
    int sensitivity; //0=lowest, 2=highest
    int decimation; //decimation level.  Affects RBW
    int iflo_path; //0 = use 10.7MHz IF LO, 1 = use 2.9MHz IF LO.
    int adcclk_path; //0 = ADC uses a 23-1/3MHz clock, 1 = ADC uses a 22.5MHz clock.
    int deviceid; //device ID, 0-7, seems to be disabled in the linux API
    bool docal; //if true, the following cal data will be used for calculations
    unsigned char caldata[4096]; //cal data to use for calculations
    bool preset;  //attempt to preset the Signal hound after configuring
    bool ext_ref; //attempt to configure a 10MHz input clock
    bool preamp; //attempt to init preamp (SA44B only)
    int ext_trigger; //set trigger to external, freerunning, or pulsed output

    //Temperate compensation.  Not implemented as I dont have the API for it.
    bool dotemp;
    char *filename;

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
      ext_trigger = 0; //is this right?  I am not sure zero is a good setting.  Should be SHAPI_TRIGGERNORMAL
      dotemp = false;
    }
  };

  struct rfOpts {
    bool slowSweep;  //True performs slow sweep.  False performs a fast sweep
    double start_freq; //Sweep beginning frequency
    double stop_freq; //Sweep ending frequency
    int fftsize; //how large should the FFT window be
    int image_rejection; // 0 = mask both high and low, 1 = masks high side, 2 = masks low side
    int average;  //only used in slow sweep.  Arg is the number of ffts to average.  (fftsize*average) % 512 === 0
    rfOpts() {
      slowSweep = true;
      start_freq = 150.0e6;
      stop_freq = MAX_FREQ;
      fftsize = 1024;
      image_rejection = 0;
      average = 16;
    }
  };

  class SignalHound {
    public:
      explicit SignalHound( struct configOpts *co = 0 );
      ~SignalHound();
      /** \brief Returns a human readable string of the current configuration options
       *
       * Returns a human readable string of the configuration options, as well
       * as a few calculated items like the Resolution Bandwith (RBW), sweep
       * count size, and Signal Hound Temperatures.
       *
       * @see SignalHound::SignalHound::temperature
      */
      std::string info( void );
      int  initialize( struct configOpts *co = 0 );

      /** \brief Returns an array of frequencies that can be used as the titles.*/
      std::vector<int> frequencies();

      /** \brief Returns the size of array needed to perfom the sweep.*/
      double temperature();

      /** \brief Returns the Resolution Bandwidth*/
      double calcRBW( void ) { return ( 1.6384e6 / rfopts.fftsize / ( rfopts.slowSweep ? opts.decimation : 1 ) ); }

      /** \brief Returns the size of array needed to perfom the sweep.*/
      int sweepCount();

      /** \brief Returns the size of array needed to perfom the sweep.*/
      double sweepTime();

      /** \brief Perform a frequency sweep.
           *
       * Returns the size of the array returns, 0 if incorrectly
       * configured, and -errno if the sweep failed.  Passing a
       * rfopts will use the specific configuration.
       *
       * @see SignalHound::SignalHound::rfopts
       * @see SignalHound::SignalHound::sweep
      */
      int sweep();
      int sweep( struct rfOpts rfopts );

      /** \brief Returns True if the configuration settings are allowable
       *
       * Returns true if a sweep can be performed with the settings as currently
       * configured in SignalHound::SignalHound::rfopts. If it returns false, a
       * string error message will be placed in errmsg.  If true, errmsg will be
       * set to "No Errors".  Internally, the
       *
       * @see SignalHound::SignalHound::rfopts
       * @see SignalHound::SignalHound::sweep
      */
      bool verfyRFConfig( std::string &errmsg, struct rfOpts ropts );

      /** \brief Structure that contains all the pertinant information to run a sweep*/
      struct rfOpts rfopts;
      std::vector<double> powers;


    private:
      struct configOpts opts;
      //    std::vector<double> powers;
      int errno;
      std::string errmsg;
      unsigned char *sighound_struct; //allocated on start up


  };
}