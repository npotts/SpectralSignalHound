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
  SHBackendSQLite::SHBackendSQLite(bool &ok, std::string dbfilename): SHBackend(ok, dbfilename), metadata_insert_proto(METADATA_INSERT) {
    logger = getSignalHoundLogger("SQLBackend");
    pDB = NULL;
    pStmt = NULL;
    CLOG(DEBUG, "SQLBackend") << "Meta Data Insert Prototype: " << metadata_insert_proto;
    ok = setOutput(dbfilename);
  }
  bool SHBackendSQLite::setOutput(std::string dbfilename) {
    if (pDB) {
      pDB->Close();
      delete(pDB);
    }
    try {
      pDB = new Kompex::SQLiteDatabase(dbfilename, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
      // create statement instance for sql queries/statements
      pStmt = new Kompex::SQLiteStatement(pDB);
      CLOG(DEBUG, "SQLBackend") << "Using SQLite Version: " << pDB->GetLibVersionNumber();
      pStmt->SqlStatement(METADATA_TABLE_CREATE); //create sweep_metadata table if it doesnt exist
      return true;
    } catch (Kompex::SQLiteException &e) {
      CLOG(ERROR, "SQLBackend") << "Unable to create metadata table: " << e.GetErrorDescription();
      return false;
    }
    return false;
  }
  bool SHBackendSQLite::newSweep(CMySignalHound &sighound) {
    /*A new sweep is about to take place.  Add a new entry to the sweep_metadata table,
      create a new table for the sweep data, and adjust the internal data_table to point
      to the new table for further calls to SHBackendSQLite::addSweep() */

    //setup data table name.  We prefix "slow" for slow sweep, "fast" for fast, "rbw" for 5Mhz rbw,
    //"zspan" for zero span, "pn" for phasenoise.  The others default to "unknown"
    switch(sighound.m_settings.m_sweepMode) {
      case HOUND_SWEEP_MODE_SLOW_SWEEP: data_table = "slow"; break;
      case HOUND_SWEEP_MODE_FAST_SWEEP: data_table = "fast"; break;
      case HOUND_SWEEP_MODE_RBW_5MHz: data_table = "rbw"; break;
      case HOUND_SWEEP_MODE_ZERO_SPAN: data_table = "zspan"; break;
      case HOUND_SWEEP_MODE_TRACK_GEN: data_table = "trackgen"; break;
      case HOUND_SWEEP_MODE_PHASE_NOISE: data_table = "pn"; break;
      default: data_table = "unknown"; break;
    }
    data_table += currentTimeDate(false, "_%Y%m%dL%H%M%S");
    CLOG(DEBUG, "SQLBackend") << "Binding() values to query to populate" << data_table;
    try {
      pStmt->Sql(metadata_insert_proto); //preload statement.  Now to bind
      int i=1;
      //rowid, data_table, timestamp, m_startFreq
      pStmt->BindString(i++, data_table);
      pStmt->BindString(i++, currentTimeDate());
      pStmt->BindDouble(i++, sighound.m_settings.m_startFreq);
      pStmt->BindDouble(i++, sighound.m_settings.m_stopFreq);
      pStmt->BindDouble(i++, sighound.m_settings.m_centerFreq);
      pStmt->BindDouble(i++, sighound.m_settings.m_spanFreq);
      pStmt->BindDouble(i++, sighound.m_settings.m_stepFreq);
      pStmt->BindDouble(i++, sighound.m_settings.m_stepAmpl);
      pStmt->BindDouble(i++, sighound.m_settings.m_refLevel);
      pStmt->BindDouble(i++, sighound.m_settings.m_refLevelOffset);
      pStmt->BindInt(i++, sighound.m_settings.m_refUnitsmV);
      pStmt->BindDouble(i++, sighound.m_settings.m_logDbDiv);
      pStmt->BindInt(i++, sighound.m_settings.m_attenIndex);
      pStmt->BindBool(i++, sighound.m_settings.m_scaleLin);
      pStmt->BindInt(i++, sighound.m_settings.m_AmplUnits);
      pStmt->BindBool(i++, sighound.m_settings.m_signalTrackOn);
      pStmt->BindInt(i++, sighound.m_settings.m_vidAvg);
      pStmt->BindBool(i++, sighound.m_settings.m_TrigVideo);
      pStmt->BindDouble(i++, sighound.m_settings.m_TrigPos);
      pStmt->BindInt(i++, sighound.m_settings.m_CalOutSyncTrig);
      pStmt->BindDouble(i++, sighound.m_settings.m_videoTriggerLevel);
      pStmt->BindInt(i++, sighound.m_settings.m_ZSMode);
      pStmt->BindInt(i++, sighound.m_settings.m_RBWSetpoint);
      pStmt->BindInt(i++, sighound.m_settings.m_VBWSetpoint);
      pStmt->BindInt(i++, sighound.m_settings.m_VDMMA);
      pStmt->BindInt(i++, sighound.m_settings.m_VDMode);
      pStmt->BindBool(i++, sighound.m_settings.m_UseExtRef);
      pStmt->BindBool(i++, sighound.m_settings.m_RBWIsAuto);
      pStmt->BindBool(i++, sighound.m_settings.m_VBWIsAuto);
      pStmt->BindInt(i++, sighound.m_settings.m_SWPTMSetpoint);
      pStmt->BindBool(i++, sighound.m_settings.m_maxHold);
      pStmt->BindBool(i++, sighound.m_settings.m_suppressImage);
      pStmt->BindInt(i++, sighound.m_settings.m_decimation);
      pStmt->BindInt(i++, sighound.m_settings.m_MarkerSelected);
      pStmt->BindInt(i++, sighound.m_settings.m_sweepMode);
      pStmt->BindDouble(i++, sighound.m_settings.m_sweepTime);
      pStmt->BindInt(i++, sighound.m_settings.m_Averaging);
      pStmt->BindInt(i++, sighound.m_settings.m_DetectorPasses);
      pStmt->BindInt(i++, sighound.m_settings.m_SubTraceCount);
      pStmt->BindInt(i++, sighound.m_settings.m_FFTSize);
      pStmt->BindInt(i++, sighound.m_settings.m_traceSize);
      pStmt->BindDouble(i++, sighound.m_settings.m_ExtMixerOffset);
      pStmt->BindDouble(i++, sighound.m_settings.m_ZSFreqPeak);
      pStmt->BindDouble(i++, sighound.m_settings.m_ZSSweepTime);
      pStmt->BindInt(i++, sighound.m_settings.m_SweepsToDo);
      pStmt->BindInt(i++, sighound.m_settings.m_Overpowered);
      pStmt->BindInt(i++, sighound.m_settings.m_PreampOn);
      pStmt->BindInt(i++, sighound.m_settings.m_PNStartDecade);
      pStmt->BindInt(i++, sighound.m_settings.m_PNStopDecade);
      pStmt->BindDouble(i++, sighound.m_channelBW);
      pStmt->BindDouble(i++, sighound.m_channelSpacing);
      pStmt->BindInt(i++, sighound.m_BBSPSetpt);
      pStmt->BindInt(i++, sighound.m_serialNumber);
      pStmt->BindDouble(i++, sighound.m_HzPerPt);
      pStmt->BindInt(i++, sighound.m_SubTraceSize);

      pStmt->ExecuteAndFree();
      CLOG(DEBUG, "SQLBackend") << "Metadata successfully inserted";
    } catch (Kompex::SQLiteException &e) {
      CLOG(ERROR, "SQLBackend") << "Metadata unable to be inserted";
      CLOG(ERROR, "SQLBackend") << "Reason given: " << e.GetErrorDescription();
      return false;
    }

    //CLOG_IF(sighound.m_traceSize + 3 > 2000, WARNING, "SQLBackend") <<  "Sweep results contains" << sighound.m_traceSize + 3 << "columns, which is more than the default of 2000 that standard builds of SQLite supports. Because of this, standard builds of software with SQLite might not be able to properly read the database file.  You might want to consider using a CSV backend or changing the RBW / VBR.";

    if (sighound.m_traceSize + 4 > 2000) {
      CLOG(WARNING, "SQLBackend") <<  "Sweep results contains" << sighound.m_traceSize + 4 << "columns, which is more than the default of 2000 that standard builds of SQLite supports. Because of this, the database schema will change slightly. You might want to consider using a CSV backend or changing the RBW / VBR.";
      std::string statement("CREATE TABLE [" + data_table + "] (rowid INTEGER NOT NULL PRIMARY KEY, timestamp DEFAULT CURRENT_TIMESTAMP NOT NULL, header_row BOOLEAN DEFAULT FALSE NOT NULL, temperature DOUBLE NOT NULL, csv TEXT NOT NULL)");
      std::stringstream headers;
      for (int i=0; i<sighound.m_traceSize; i++)
        headers << (i == 0 ? "": ",") << (unsigned long long int) sighound.GetFrequencyFromIdx(i);
      std::string insheaders = "INSERT INTO [" + data_table + "] VALUES (NULL, '" + currentTimeDate() + "', 'true', 'N/A', '" + headers.str() + "')";
      try {
        // CLOG(DEBUG, "SQLBackend") << statement;
        // CLOG(DEBUG, "SQLBackend") << insheaders;
        pStmt->SqlStatement(statement);
        pStmt->BeginTransaction();
        pStmt->Sql(insheaders);
        pStmt->ExecuteAndFree();
        pStmt->CommitTransaction();
        return true;
      } catch (Kompex::SQLiteException &e) {
        CLOG(ERROR, "SQLBackend") << "Could not create needed sweep table";
        CLOG(ERROR, "SQLBackend") << "Reason given:" << e.GetErrorDescription();
      }
    } else { //Less than 2000 columns
      CLOG(DEBUG, "SQLBackend") <<  "Sweep results contains" << sighound.m_traceSize + 4 << "columns.  Using normal storage mechanism.";
      std::stringstream create;
      create << "CREATE TABLE [" + data_table + "] (rowid INTEGER NOT NULL PRIMARY KEY, timestamp DEFAULT CURRENT_TIMESTAMP NOT NULL, header_row BOOLEAN DEFAULT FALSE NOT NULL, temperature DOUBLE NOT NULL";
      for (int i=0; i<sighound.m_traceSize; i++) {
        create << ", [" << (unsigned long long int) sighound.GetFrequencyFromIdx(i) << "] DOUBLE";
      }
      create << ")";
      try { //create new Table.  At this point, we should have the number of frequency
        pStmt->SqlStatement(create.str());
        return true;
      } catch (Kompex::SQLiteException &e) {
        CLOG(ERROR, "SQLBackend") << "Could not create table" + data_table;
        CLOG(ERROR, "SQLBackend") << "Reason given: " << e.GetErrorDescription();
      }
    }
    return false; //if we get here, something is wrong
  }
  bool SHBackendSQLite::addSweep(std::vector<double> dbvalues) {
    CLOG(DEBUG, "SQLBackend") << "Inserting Data";
    std::stringstream data;

    //yeah yeah yeah. I know this may be open to SQL injection, but when working with large quanties, we 
    //can easily overflow the number of '?' binds allowed in stock SQLite instances
    if (dbvalues.size() + 4 > 2000) {
      //Large sweep, store whole result thing  into a single column.
      data << "INSERT INTO [" << data_table << "] VALUES (NULL, '" << currentTimeDate() <<  "', 'false'," << dbvalues.at(0) << ", '";
      for(unsigned int i=1; i < dbvalues.size(); i++)
        data << (i == 1 ? "" : ", ") << dbvalues.at(i);
      data << "')";
    } else {
      data << "INSERT INTO [" << data_table << "] VALUES (NULL";
      data << ", '" << currentTimeDate() <<  "', 'false'";
      for(unsigned int i=0; i < dbvalues.size(); i++)
        data << ", " << dbvalues.at(i);
      data << ")";
    }
    try {
      pStmt->BeginTransaction();
      pStmt->Sql(data.str());
      pStmt->ExecuteAndFree();
      pStmt->CommitTransaction();
      return true;
    } catch (Kompex::SQLiteException &e) {
      CLOG(ERROR, "SQLBackend") << "Could not insert into " + data_table;
      CLOG(ERROR, "SQLBackend") << "Reason given: " << e.GetErrorDescription();
      exit(1);
      return false;
    }
    return false;
  }
};