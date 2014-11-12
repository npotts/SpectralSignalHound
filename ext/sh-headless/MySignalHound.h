/**

File: MySignalHound.h
Copyright 2012 Test Equipment Plus
Author: Justin Crooks
E-Mail: justin@testequipmentplus.com
Description: Header file for CMySignalHound Class
			which encapsulates the functionality of the Signal Hound

Revision History:
12 JAN 2012		Justin Crooks		Created from Signal Hound code.  Removed Windows dependencies.
19 JAN 2012		Justin Crooks		Added access to min & max RBW setpoints

*/
#include "Settings.h"
#include "MeasRcvr.h"

#pragma once


double mW2dBm(double mWIn);
double GetRBWFromIndex(int idx);

class CMySignalHound
{
public:
	CMySignalHound(void);
	~CMySignalHound(void);

	int Initialize(void);

	int SetBestClock(void);
	int SetupForSweep(void);
	int GetBestSensitivity(void);
	int PreamplifierOnOff(bool onoff);

	int CheckRBWVBW(void);
	int SetCenterAndSpan(double cf, double span);
	int SetStartAndStop(double start, double stop);
	int GetMaxFFTPowerFromRBW(double RBW);
	int GetMinFFTPowerFromRBW(double RBW);
	bool ForceFastSweep(double span, int swptmsetpt);
	double GetFrequencyFromIdx(int idx);
	int SetExternalRef();

	double GetACPL();//Channel power functions
	double GetACPR();
	double GetCP();

	int DoSweep();
	int DoPhaseNoiseSweep(int startDecade, int stopDecade, double referenceAmplitude);

	double ReadTemperature();
	int DoMeasuringReceiver(MEAS_RCVR_STRUCT * pMeasRcvr);
	void * OpenAudio();
	void CloseAudio(void * pAudioStr);

public:
	CSettings m_settings;
	double * pDataMin;
	double * pDataMax;

	double	m_channelBW;
	double	m_channelSpacing;
	int m_BBSPSetpt;
	int m_serialNumber;

	double m_HzPerPt;
	int m_traceSize;
	int m_SubTraceSize;
    int m_idxOfminRBW;
    int m_idxOfmaxRBW;


};

