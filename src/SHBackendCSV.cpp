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

#include "SHBackendCSV.h"

namespace SignalHound {
  SHBackendCSV::~SHBackendCSV() {
    el::Loggers::unregisterLogger("CSVBackend");
  }
  SHBackendCSV::SHBackendCSV(bool &ok, std::string dbfilename): SHBackend(ok, dbfilename) {
    logger = getSignalHoundLogger("CSVBackend");
    ok = setOutput(dbfilename);
  }
  bool SHBackendCSV::setOutput(std::string dbfilename) {
    if (csv.is_open())
      csv.close();
    csv.open(dbfilename.c_str());
    return csv.is_open();
  }
  bool SHBackendCSV::newSweep(CMySignalHound &sighound) {
    /**< A new sweep is about to take place - Write out the headers.*/
    try {
      CLOG(DEBUG, "CSVBackend") << "Initializing new CSV file";
      csv << "timestamp,temperature";
      for(int i=0; i<sighound.m_traceSize; i++)
        csv << "," << (int) sighound.GetFrequencyFromIdx(i);
      csv << std::endl;
      CLOG(INFO, "CSVBackend") << "CSV headers written: total of" << sighound.m_traceSize << "points per trace";
      return true;
    } catch (std::exception &e) {
      CLOG(FATAL, "CSVBackend") << "Unable to write CSV headers: " << e.what();
    }
    return false;
  }
  bool SHBackendCSV::addSweep(std::vector<double> dbvalues) {
    try {
      CLOG(DEBUG, "CSVBackend") << "Inserting Data";
      csv << currentTimeDate();
      for(unsigned int i=0; i < dbvalues.size(); i++)
        csv << "," << dbvalues.at(i);
      csv << std::endl << std::flush;
      return true;
    } catch (std::exception &e) {
      CLOG(FATAL, "CSVBackend") << "Error writing data: " << e.what();
    }
    return false;
  }
};