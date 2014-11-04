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

#include "SHBackendSQLite.h"

namespace SignalHound {
  SHBackendSQLite::~SHBackendSQLite() {
    el::Loggers::unregisterLogger("SQLBackend");
  }
  SHBackendSQLite::SHBackendSQLite(bool &ok, std::string dbfilename): SHBackend(ok, dbfilename) {
    logger = el::Loggers::getLogger("SQLBackend");
    pDB = NULL;
    pStmt = NULL;
    ok = setOutput(dbfilename);
  }
  bool SHBackendSQLite::setOutput(std::string dbfilename) {
    pDB = new Kompex::SQLiteDatabase(dbfilename, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
    // create statement instance for sql queries/statements
    pStmt = new Kompex::SQLiteStatement(pDB);
    CLOG(DEBUG, "SQLBackend") << "Using SQLite Version: " << pDB->GetLibVersionNumber();
    //create sweep_metadata table
    pStmt->SqlStatement("CREATE TABLE IF NOT EXISTS sweep_metadata (rowid INTEGER NOT NULL PRIMARY KEY, timestamp ,attenuation  DOUBLE NOT NULL, mixerband INT NOT NULL, sensitivity INT NOT NULL, decimation INT NOT NULL, iflo_path INT NOT NULL, adcclk_path INT NOT NULL, deviceid INT NOT NULL, docal INT NOT NULL, caldata NOT NULL, preset NOT NULL, ext_ref INT NOT NULL, preamp INT NOT NULL, ext_trigger INT NOT NULL, dotemp INT NOT NULL, temp_calfname INT NOT NULL, slowsweep INT NOT NULL, start_freq INT NOT NULL, stop_freq INT NOT NULL, fftsize INT NOT NULL, image_rejection INT NOT NULL, average INT NOT NULL, settingsvalid INT NOT NULL, temperature INT NOT NULL,  rbw INT NOT NULL, sweeppoints INT NOT NULL, sweeptime DOUBLE NOT NULL)");
    return true;
  }
  bool SHBackendSQLite::setFreqColumns(std::vector<int> columns, std::string *postfix) {
    /* Setup table.  Postfix is some string to tack onto the end of the table name
       functionally, there are 2 tables involved.  1 is the "sweep_metadata" table which
       contains all the sweep parameters and a reference to the table that contains the data
       ie: start_freq, stop_freq, ... , sweep_start_time, sweep_table.  The second table is 
       `sweep_table` and it contains all the raw data with a couple beginning columns:
       rowid, timestamp, freq_1, freq_2, freq_3, ... freq_n;  These tables may have variable
       data lengths. (due to differing frequency values)
    */

       /*pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (4, 'Lehmann', 'Carlene ', 17, 50.8)");

    // ---------------------------------------------------------------------------------------------------------
    // insert some data with Bind..() methods
    pStmt->Sql("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES(?, ?, ?, ?, ?);");
    pStmt->BindInt(1, 5);
    pStmt->BindString(2, "Murahama");
    pStmt->BindString(3, "Yura");
    pStmt->BindInt(4, 28);
    pStmt->BindDouble(5, 60.2);
    */
       return true;
  }
  bool SHBackendSQLite::newSweep(struct configOpts &opt, struct rfOpts &rfopt) {
    /*A new sweep is about to take place.  Add a new entry to the sweep_metadata table,
      create a new table for the sweep data, and adjust the internal data_table to point
      to the new table for further calls to SHBackendSQLite::addSweep() */
      return true;
  }
  bool SHBackendSQLite::insertData(std::vector<double> dbvalues) {
    return true;
  }
};