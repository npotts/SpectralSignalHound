/*
 * Copyright (c) 2014, Nick Potts
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * * Neither the name of SpectralSignalHound nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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
#include <sstream>
#include <vector>
#include "SHBackend.h"

#include "../ext/kompex/include/KompexSQLiteBlob.h"
#include "../ext/kompex/include/KompexSQLiteStatement.h"
#include "../ext/kompex/include/KompexSQLiteDatabase.h"
#include "../ext/kompex/include/KompexSQLiteException.h"

using namespace std;
namespace SignalHound {
 typedef std::vector<std::string> vstr;

 #define METADATA_TABLE_CREATE "CREATE TABLE IF NOT EXISTS sweep_metadata (rowid INTEGER NOT NULL PRIMARY KEY, data_table TEXT, timestamp DEFAULT CURRENT_TIMESTAMP NOT NULL, m_startFreq DOUBLE NOT NULL, m_stopFreq DOUBLE NOT NULL, m_centerFreq DOUBLE NOT NULL, m_spanFreq DOUBLE NOT NULL, m_stepFreq DOUBLE NOT NULL, m_stepAmpl DOUBLE NOT NULL, m_refLevel DOUBLE NOT NULL, m_refLevelOffset DOUBLE NOT NULL, m_refUnitsmV INTEGER NOT NULL, m_logDbDiv DOUBLE NOT NULL, m_attenIndex INTEGER NOT NULL, m_scaleLin BOOLEAN NOT NULL, m_AmplUnits INTEGER NOT NULL, m_signalTrackOn BOOLEAN NOT NULL, m_vidAvg INTEGER NOT NULL, m_TrigVideo BOOLEAN NOT NULL, m_TrigPos DOUBLE NOT NULL, m_CalOutSyncTrig INTEGER NOT NULL, m_videoTriggerLevel DOUBLE NOT NULL, m_ZSMode INTEGER NOT NULL, m_RBWSetpoint INTEGER NOT NULL, m_VBWSetpoint INTEGER NOT NULL, m_VDMMA INTEGER NOT NULL, m_VDMode INTEGER NOT NULL, m_UseExtRef BOOLEAN NOT NULL, m_RBWIsAuto BOOLEAN NOT NULL, m_VBWIsAuto BOOLEAN NOT NULL, m_SWPTMSetpoint INTEGER NOT NULL, m_maxHold BOOLEAN NOT NULL, m_suppressImage BOOLEAN NOT NULL, m_decimation INTEGER NOT NULL, m_MarkerSelected INTEGER NOT NULL, m_sweepMode INTEGER NOT NULL, m_sweepTime DOUBLE NOT NULL, m_Averaging INTEGER NOT NULL, m_DetectorPasses INTEGER NOT NULL, m_SubTraceCount INTEGER NOT NULL, m_FFTSize INTEGER NOT NULL, m_ExtMixerOffset DOUBLE NOT NULL, m_ZSFreqPeak DOUBLE NOT NULL, m_ZSSweepTime DOUBLE NOT NULL, m_SweepsToDo INTEGER NOT NULL, m_Overpowered INTEGER NOT NULL, m_PreampOn INTEGER NOT NULL, m_PNStartDecade INTEGER NOT NULL, m_PNStopDecade INTEGER NOT NULL, m_channelBW DOUBLE NOT NULL, m_channelSpacing DOUBLE NOT NULL, m_BBSPSetpt INTEGER NOT NULL, m_serialNumber INTEGER NOT NULL, m_HzPerPt DOUBLE NOT NULL, m_traceSize INTEGER NOT NULL, m_SubTraceSize INTEGER NOT NULL)"
 #define METADATA_INSERT "INSERT INTO sweep_metadata (rowid, data_table, timestamp, m_startFreq, m_stopFreq, m_centerFreq, m_spanFreq, m_stepFreq, m_stepAmpl, m_refLevel, m_refLevelOffset, m_refUnitsmV, m_logDbDiv, m_attenIndex, m_scaleLin, m_AmplUnits, m_signalTrackOn, m_vidAvg, m_TrigVideo, m_TrigPos, m_CalOutSyncTrig, m_videoTriggerLevel, m_ZSMode, m_RBWSetpoint, m_VBWSetpoint, m_VDMMA, m_VDMode, m_UseExtRef, m_RBWIsAuto, m_VBWIsAuto, m_SWPTMSetpoint, m_maxHold, m_suppressImage, m_decimation, m_MarkerSelected, m_sweepMode, m_sweepTime, m_Averaging, m_DetectorPasses, m_SubTraceCount, m_FFTSize, m_traceSize, m_ExtMixerOffset, m_ZSFreqPeak, m_ZSSweepTime, m_SweepsToDo, m_Overpowered, m_PreampOn, m_PNStartDecade, m_PNStopDecade, m_channelBW, m_channelSpacing, m_BBSPSetpt, m_serialNumber, m_HzPerPt, m_SubTraceSize) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

 class SHBackendSQLite: public SHBackend {
  public:
   ~SHBackendSQLite();
   SHBackendSQLite(bool &ok, std::string);
   bool setOutput(std::string);
   bool newSweep(CMySignalHound &);
   bool addSweep(std::vector<double>);
  private:
   std::string data_table, metadata_insert_proto, sweep_insert_proto;
   Kompex::SQLiteDatabase *pDB;
   Kompex::SQLiteStatement *pStmt;
   el::Logger* logger;

 };
}