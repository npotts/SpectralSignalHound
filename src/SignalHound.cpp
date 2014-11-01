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

namespace SignalHound {
  SignalHound::~SignalHound() {
    if (errno >= 0) {
      SHAPI_CyclePowerOnExit(sighound_struct);
      SHAPI_Close(sighound_struct);
    }
    free(sighound_struct);
  }
  SignalHound::SignalHound(struct configOpts *co) {
    //Initialize and create the signal hound
    if (co)
      memcpy(&opts, co, sizeof(opts));
    errno = -1;
    int sh_size = SHAPI_GetStructSize();
    sighound_struct = ( unsigned char * ) malloc( sh_size * sizeof(double) );
    //check malloc
    if (sighound_struct == NULL)
      std::cerr << "malloc() failed?  The OOM killer is lurking nearby...." << std::endl;
  }
  int SignalHound::initialize(struct configOpts *co) {
    if (co)
      memcpy(&opts, co, sizeof(opts));
    //initialize.  Check if we can short cut with cal data
    if (opts.docal) { //yes, take the shortcut
      errno = SHAPI_InitializeEx(sighound_struct, opts.caldata);
    } else {
      errno = SHAPI_Initialize(sighound_struct);
      //copy cal data over now hat we have it
      if (errno == 0) {
        SHAPI_CopyCalTable(sighound_struct, opts.caldata);
        opts.docal = true;
      }
    }
    if (errno != 0) return errno;

    if (opts.preset) { //Preset the unit
      if ( (errno = SHAPI_CyclePort(sighound_struct)) != 0)
        return errno;
    }

    //now configure
    if ( (errno=SHAPI_Configure(sighound_struct,
                             opts.attenuation,
                             opts.mixerBand,
                             opts.sensitivity,
                             opts.decimation,
                             opts.iflo_path,
                             opts.adcclk_path)) != 0)
      return errno;
    //Setup External Ref if asked
    if (opts.ext_ref && ( (errno = SHAPI_SelectExt10MHz(sighound_struct)) != 0))
      return errno;

    if (opts.preamp)
      SHAPI_SetPreamp(sighound_struct, 1);
    else
      SHAPI_SetPreamp(sighound_struct, 0);
    
    //rudimentary parameter validation
    if ((opts.ext_trigger != SHAPI_EXTERNALTRIGGER) && (opts.ext_trigger != SHAPI_SYNCOUT) && (opts.ext_trigger != SHAPI_TRIGGERNORMAL))
      opts.ext_trigger = SHAPI_TRIGGERNORMAL;
    SHAPI_SyncTriggerMode(sighound_struct, opts.ext_trigger);
    return errno;
  }
  double SignalHound::temperature() {
    if (!errno)
      return (double) SHAPI_GetTemperature(sighound_struct);
    return (-9999.9999);
  }
  int SignalHound::sweepCount() {
    if (errno) return -1;
    return rfopts.slowSweep ? SHAPI_GetSlowSweepCount(sighound_struct, rfopts.start_freq, rfopts.stop_freq, rfopts.fftsize):\
                              SHAPI_GetFastSweepCount(rfopts.start_freq, rfopts.stop_freq, rfopts.fftsize);
  }
  bool SignalHound::verfyRFConfig(std::string &errmsg, struct rfOpts ropts) {
    //check if we are doing a slow or fast sweep
    bool ok=true;
    ok &= (ropts.stop_freq > ropts.start_freq); 
    if (!ok) {errmsg = "Stop Frequency must be higher than the Start Frequency"; return false;}
    if (ropts.slowSweep) {
      //check FFT length
      ok &= ((ropts.fftsize > 15) && (ropts.fftsize <= 65536) && ( (ropts.fftsize & (ropts.fftsize - 1)) == 0));
      if (!ok) {errmsg = "FFT Size must be in the range of [16, 65536] and a power of 2."; return false;}
      ok &= ((ropts.average * ropts.fftsize) % 512 == 0);
      if (!ok) {errmsg = "FFT Size * Average must be a multiple of 512"; return false;}
    } else {
      ok &= (ropts.fftsize == 1) | ((ropts.fftsize > 2) && (ropts.fftsize <= 256) && ((ropts.fftsize & (ropts.fftsize - 1)) == 0));
      if (!ok) {errmsg = "FFT Size must be either 1 for raw sample, or in the range of [16, 256] and a power of 2."; return false;}
    }
    return ok;
  }
  int SignalHound::sweep(struct rfOpts o) {
    rfopts = o;
    return sweep();
  }
  int SignalHound::sweep(void) {
    if (!verfyRFConfig(errmsg, rfopts)) return 0;
    //make sure the vectors are large enough
    int req_size = sweepCount(), sweep_return=0;
    powers.reserve(req_size > 0 ? req_size : 0);

    if (rfopts.slowSweep) {
      sweep_return =  SHAPI_GetSlowSweep(sighound_struct, 
                                         powers.data(),
                                         rfopts.start_freq,
                                         rfopts.stop_freq,
                                         &req_size,
                                         rfopts.fftsize,
                                         rfopts.average,
                                         rfopts.image_rejection);
      if (sweep_return != 0)
        return -sweep_return;
    } else {
      SHAPI_GetFastSweep(sighound_struct,
                         powers.data(),
                         rfopts.start_freq,
                         rfopts.stop_freq,
                         &req_size,
                         rfopts.fftsize,
                         rfopts.image_rejection);
      if (sweep_return != 0)
        return -sweep_return;
    return req_size;
    }
    return -1;
  }
  std::vector<double> SignalHound::measurement_frequencies(void) {
    //return a vector of frequencies based on the current configuration.
    std::vector<double> rtn;  rtn.clear();
    int req_size = sweepCount();
    if (!verfyRFConfig(errmsg, rfopts)) return rtn;
    if (rfopts.slowSweep) {
      for (int i=0; i < req_size; i++)
        rtn.push_back(rfopts.start_freq + i * 486111.111 / rfopts.fftsize / opts.decimation);
    } else { //fast sweep is a little different
      if (rfopts.fftsize == 1){
        for(int i = 0; i<req_size; i++)
          rtn.push_back(rfopts.start_freq + i*400.0e3/rfopts.fftsize);
      } else {
        for(int i = 0; i < req_size; i++)
          rtn.push_back(rfopts.start_freq + i*2.0e5);
      }
    }
    return rtn;
  }
  std::string SignalHound::info() {
    //create a formmatted string of status data
    std::stringstream rtn;
    int samples = sweepCount();
    rtn << "Frequency Start:                \t" << (unsigned long) rfopts.start_freq << " Hz" << std::endl;
    rtn << "Frequency Stop:                 \t" <<  (unsigned long) rfopts.stop_freq << " Hz" << std::endl;
    rtn << "Frequency Span:                 \t" <<  (unsigned long) (rfopts.stop_freq - rfopts.start_freq) << " Hz" << std::endl;
    rtn << "Center Frequency (Mean):        \t" << (unsigned long) ((rfopts.stop_freq - rfopts.start_freq)/2.0) << " Hz" << std::endl;
    rtn << "Center Frequency (Geometric):   \t" << (unsigned long) sqrt(rfopts.stop_freq*rfopts.start_freq) << " Hz" << std::endl;
    rtn << "Resolution Bandwith:            \t" << calcRBW() << " Hz" << std::endl;
    rtn << "FFT Width:                      \t" << rfopts.fftsize << std::endl;
    rtn << "Image Rejection:                \t" << (rfopts.image_rejection == 0 ? "On": (rfopts.image_rejection == 1 ? "High Side Only" : "Low Side Only")) << std::endl;
    rtn << "Slow Sweep Average:             \t" << rfopts.average << std::endl;
    rtn << "Points per Sweep:               \t" << samples << std::endl;
    rtn << "Sweep Type:                     \t" << (rfopts.slowSweep ? "Slow" : "Fast") << std::endl;
    rtn << "Attenuation (dBm):              \t" << (int) opts.attenuation << std::endl;
    rtn << "Sensitivity Level:              \t" << opts.sensitivity << std::endl;
    rtn << "Decimation:                     \t" << opts.decimation << std::endl;
    rtn << "IF LO:                          \t" << (opts.iflo_path == 0 ? "10.7MHz": "2.9MHz") << std::endl;
    rtn << "ADC Clock:                      \t" << (opts.adcclk_path == 0 ? "23-1/3MHz": "22.5MHz") << std::endl;
    rtn << "Device ID:                      \t" << opts.deviceid << std::endl;
    rtn << "Provided Calibration Data:      \t" << (opts.docal ? "Yes": "No") << std::endl;
    rtn << "Preset Signal Hound on Startup: \t" << (opts.preset ? "Yes": "No") << std::endl;
    rtn << "Attempt to Use External Reference:\t" << (opts.ext_ref ? "Yes": "No") << std::endl;
    rtn << "Attempt to use Preamp:          \t" << (opts.preamp ? "Yes": "No") << std::endl;
    rtn << "Signal Hound Temperature:       \t" << temperature() << std::endl;
    bool cfgok = verfyRFConfig(errmsg, rfopts);
    rtn << "Settings Valid for Sweep:       \t" << (cfgok ? "Yes" : "No") << std::endl;
    if (!cfgok)
      rtn << "Invalid Config Error Message: '" << errmsg << "'" << std::endl;
    return rtn.str();
  }
};