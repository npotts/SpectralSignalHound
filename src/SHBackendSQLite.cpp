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
    configureLoggers();
    pDB = NULL;
    pStmt = NULL;
    //build up the metadata table prototype because we have a more complete
    // information set than at compile time.
    //I dont have a more efficient way to do this, but there is a better way to do this.
    const char * metadata_params_[] = {"attenuation", "mixerband", "sensitivity", "decimation", "iflo_path", "adcclk_path", "deviceid", "docal", "preset", "ext_ref", "preamp", "ext_trigger", "slowsweep", "start_freq", "stop_freq", "span", "center_mean", "center_geometric", "fftsize", "image_rejection", "average", "valid", "temperature", "rbw", "sweep_count", "sweep_time", "sweep_step"};
    metadata_params = vstr(metadata_params_, metadata_params_+sizeof(metadata_params_)/sizeof(const char *));
    metadata_insert_proto = "INSERT INTO sweep_metadata (rowid, timestamp, data_table";
    std::string tmp ("(NULL, ?, ?");
    for(unsigned int i=0; i<metadata_params.size(); i++) {
      metadata_insert_proto += ", " + std::string(metadata_params[i]);
      tmp += ", ?";
    } metadata_insert_proto += ") VALUES " + tmp + ")";
    CLOG(DEBUG, "SQLBackend") << "Meta Data Insert Prototype: " << metadata_insert_proto;
    ok = setOutput(dbfilename);
  }
  bool SHBackendSQLite::setOutput(std::string dbfilename) {
    pDB = new Kompex::SQLiteDatabase(dbfilename, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
    // create statement instance for sql queries/statements
    pStmt = new Kompex::SQLiteStatement(pDB);
    CLOG(DEBUG, "SQLBackend") << "Using SQLite Version: " << pDB->GetLibVersionNumber();
    try {
      pStmt->SqlStatement(METADATA_TABLE_CREATE); //create sweep_metadata table if it doesnt exist
      return true;
    } catch (Kompex::SQLiteException &e) {
      CLOG(ERROR, "SQLBackend") << "Unable to create metadata table: " << e.GetErrorDescription();
      return false;
    }
    return false;
  }
  bool SHBackendSQLite::setFreqColumns(std::vector<int> columns, std::string *postfix) {
    return true;
  }
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
    


    data_table = currentTimeDate(false, "%Y%m%dT%H%M%S");
    if (postfix)SWEEP_TABLE_PARAMS
      data_table += std::string("_") + postfix;
    CLOG(DEBUG, "SQLBackend") << " Creating a new database table with the name: " << data_table;
    std::string tmp("(NULL, NULL"), sweep_table_create("CREATE TABLE " + data_table + " (rowid INTEGER NOT NULL PRIMARY KEY, timestamp DEFAULT CURRENT_TIMESTAMP NOT NULL, data_table");
    sweep_insert_proto = "INSERT INTO " + data_table + " (rowid, timestamp, data_table";

*/
  bool SHBackendSQLite::newSweep(map_str_dbl metadata) {
    /*A new sweep is about to take place.  Add a new entry to the sweep_metadata table,
      create a new table for the sweep data, and adjust the internal data_table to point
      to the new table for further calls to SHBackendSQLite::addSweep() */
      data_table = currentTimeDate(false, "sweep_%Y%m%dL%H%M%S");
      CLOG(DEBUG, "SQLBackend") << " Creating a new database table with the name: " << data_table;
      pStmt->Sql(metadata_insert_proto); //preload statement.  Now to bind
      pStmt->BindString(1, currentTimeDate());
      pStmt->BindString(2, data_table);
      for(unsigned int i=0; i<metadata_params.size(); i++) {
        pStmt->BindDouble(i+3, metadata[metadata_params.at(i)]);
      }
      try {
        pStmt->ExecuteAndFree();
        CLOG(DEBUG, "SQLBackend") << "Metadata successfully inserted";
      } catch (Kompex::SQLiteException &e) {
        CLOG(ERROR, "SQLBackend") << "Metadata unable to be inserted";
        CLOG(ERROR, "SQLBackend") << "Reason given: " << e.GetErrorDescription();
        return false;
      }

      //create new table
      try {
        pStmt->SqlStatement("CREATE TABLE [" + data_table + "] " + SWEEP_TABLE_PARAMS);
      } catch (Kompex::SQLiteException &e) {
        CLOG(ERROR, "SQLBackend") << "Could not create table" + data_table;
        CLOG(ERROR, "SQLBackend") << "Reason given: " << e.GetErrorDescription();
      }
      return false;
  }
  bool SHBackendSQLite::addSweep(std::vector<double> dbvalues) {
    CLOG(DEBUG, "SQLBackend") << "Inserting Data";
    std::stringstream data;
    pStmt->BeginTransaction();
    pStmt->Sql("INSERT INTO [" + data_table + "] (rowid, timestamp, csv) VALUES (NULL, ?, ?)");
    for(unsigned int i=0; i < dbvalues.size(); i++)
      data << dbvalues.at(i) << (i - 1 == dbvalues.size() ? "" : ",");
    CLOG(DEBUG, "SQLBackend") << "dbvalues.size()" << dbvalues.size();
    try {
      pStmt->BindString(1, currentTimeDate());
      pStmt->BindString(2, data.str());
      pStmt->ExecuteAndFree();
      pStmt->CommitTransaction();
      return true;
    } catch (Kompex::SQLiteException &e) {
      CLOG(ERROR, "SQLBackend") << "Could not insert into " + data_table;
      CLOG(ERROR, "SQLBackend") << "Reason given: " << e.GetErrorDescription();
      return false;
    }
    return false;

  }
};