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
#include <unistd.h>
#include <fstream>
#include <thread>
#include <chrono>
#include "SignalHound.h"
#include "SHBackend.h"
#include "SHBackendSQLite.h"
#include "SHBackendCSV.h"
#include <boost/program_options.hpp>

using namespace std;
namespace SignalHound {
  class SignalHoundCLI {
    public:
      SignalHoundCLI(bool &, /**< [out] true if cfg worked, false otherwise*/
               int, /**< [in] arg count, from main() */
               char *[] /**< [in] arg list*/);
      ~SignalHoundCLI();
      bool runSweeps();
    private:
      /** /brief Make sure all the variables the user can randomly input are within a valid range. */
      void forceRange();
      /** /brief Parse Command Line Arguments and configure needed structures */
      bool parseArgs(int, char *[]);
      /** /brief Start the sweep process based on configuration options */
      bool runSweep();
      /** /brief Returns a human readable string based on an error number from the signal hound */
      std::string shErrorMsg(int);

      std::string dbfname, csvfname, calout;
      bool preamp;
      bool extref;
      int pause_between_traces;
      int repetitions;
      int mode;
      CMySignalHound sighound;
      unsigned long long int reps;

      el::Logger* logger;
      SHBackendSQLite *sqlite;
      SHBackendCSV *csv;
  };
}