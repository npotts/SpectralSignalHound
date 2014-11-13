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
#include "../ext/sh-headless/MySignalHound.h"
//#include "SHLAPI.h"


#define _ELPP_THREAD_SAFE 0
#define _ELPP_STL_LOGGING 1
#define _ELPP_BOOST_LOGGING 1
#define _ELPP_STACKTRACE_ON_CRASH 1
#define _ELPP_NO_DEFAULT_LOG_FILE 1
#define _ELPP_STACKTRACE_ON_CRASH 1
#define _ELPP_DEBUG_ERRORS 0
#include "../ext/easyloggingpp/src/easylogging++.h"

namespace SignalHound {

  typedef std::map<std::string, double> map_str_dbl;

  extern bool tostdout; ///If true, logging will be written to stdout
  extern el::Level log_level; ///Level of logging to display
  /** \brief gets a custom named logging object
   *
   * If passed a non-empty string, it will create a named custom logger 
   * from the passed string in localtime format.  It also forces formatting
   * on all existing loggers.  If passed an empty string, it just reconfigures
   * existing loggers.  Two namespace variables affect the formatting.  If 
   * SignalHound::tostdout is false, stdout will be muted entirely.  All error
   * messages not written to a log (which must be setup outside of this library)
   * will be lost.  stdcout defaults to true. SignalHound::log_level is the minimum
   * log level to record messages from (TRACE > DEBUG > FATAL > ERROR > WARN > INFO). 
   * log_level defaults to el::Logging::Fatal
   */
  //extern void configLogger(
  extern el::Logger* getSignalHoundLogger(std::string  label = "");

  /** \brief Returns a string representation of the current date and time
   *
   * This low level function returns a string representation of the !LOCALTIME! 
   * time and date. include_ms will tack on milliseconds in '.%06d' format
   * you can also provide another format to pass to snprintf.  Using defaults
   * will return something like 2014-04-05 06:07:08.123456
   */
  extern std::string currentTimeDate(bool include_ms = true, const char* format = "%Y-%m-%d %H:%M:%S");
  
}