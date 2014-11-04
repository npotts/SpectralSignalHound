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


#include <string>
#include <vector>

#include "SHBackend.h"
#include "SignalHound.h"

#include "../ext/kompex/include/KompexSQLiteBlob.h"
#include "../ext/kompex/include/KompexSQLiteStatement.h"
#include "../ext/kompex/include/KompexSQLiteDatabase.h"

using namespace std;
namespace SignalHound {
  #define METADATA_TABLE_CREATE "CREATE TABLE IF NOT EXISTS sweep_metadata (rowid INTEGER NOT NULL PRIMARY KEY, TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL ,attenuation  DOUBLE NOT NULL, mixerband INT NOT NULL, sensitivity INT NOT NULL, decimation INT NOT NULL, iflo_path INT NOT NULL, adcclk_path INT NOT NULL, deviceid INT NOT NULL, docal INT NOT NULL, caldata NOT NULL, preset NOT NULL, ext_ref INT NOT NULL, preamp INT NOT NULL, ext_trigger INT NOT NULL, dotemp INT NOT NULL, temp_calfname INT NOT NULL, slowsweep INT NOT NULL, start_freq INT NOT NULL, stop_freq INT NOT NULL, fftsize INT NOT NULL, image_rejection INT NOT NULL, average INT NOT NULL, settingsvalid INT NOT NULL, temperature INT NOT NULL,  rbw INT NOT NULL, sweeppoints INT NOT NULL, sweeptime DOUBLE NOT NULL)"
  class SHBackendSQLite: public SHBackend {
    public:
      ~SHBackendSQLite();
      SHBackendSQLite(bool &ok, std::string);
      bool setOutput(std::string);
      bool setFreqColumns(std::vector<int> columns, std::string *postfix=NULL);
      bool newSweep(struct configOpts &, struct rfOpts &);
      bool insertData(std::vector<double>);
    private:
      std::string data_table;
      Kompex::SQLiteDatabase *pDB;
      Kompex::SQLiteStatement *pStmt;
      el::Logger* logger;
  };
}