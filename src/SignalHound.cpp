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
  
};