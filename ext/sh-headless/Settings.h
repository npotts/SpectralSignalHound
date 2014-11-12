
/**  

File: Settings.h  
Copyright 2012 Test Equipment Plus  
Author: Justin Crooks  
E-Mail: justin@testequipmentplus.com  
Description: Header file for CSettings Class 
			which encapsulates the settings for the Signal Hound 

Revision History:
12 JAN 2012		Justin Crooks		Created from Signal Hound code.  Removed Windows dependancies.

*/   

#include <stdlib.h>

#pragma once

#define MAX_FREQ 4.4e9

// Modes for Zero Span				( m_ZSMode )
#define HOUND_ZS_MODE_AMPLITUDE			0
#define HOUND_ZS_MODE_FREQUENCY			1
#define HOUND_ZS_MODE_PHASE				2

// Modes for Video Detector Min Max ( m_VDMMA )
#define HOUND_VDMMA_MIN_MAX				1
#define HOUND_VDMMA_MIN_ONLY			2
#define HOUND_VDMMA_MAX_ONLY			3
#define HOUND_VDMMA_AVERAGE_ONLY		4

// Modes for Video Detector Processing ( m_VDMode )
#define HOUND_PROCESS_AS_POWER			1
#define HOUND_PROCESS_AS_VOLTAGE		2
#define HOUND_PROCESS_AS_LOG			3
#define HOUND_PROCESS_BYPASSED			4

// Sweep Modes ( m_sweepMode )
#define HOUND_SWEEP_MODE_SLOW_SWEEP		0
#define HOUND_SWEEP_MODE_FAST_SWEEP		1
#define HOUND_SWEEP_MODE_RBW_5MHz		2
#define HOUND_SWEEP_MODE_ZERO_SPAN		3
#define HOUND_SWEEP_MODE_TRACK_GEN		5
#define HOUND_SWEEP_MODE_PHASE_NOISE	7
#define HOUND_SWEEP_MODE_BB_SIG_PEAK	9

// ZERO SPAN "IF Bandwidth" ( in m_RBWSetpoint ) when m_sweepMode = HOUND_SWEEP_MODE_ZERO_SPAN
#define HOUND_IFBW_240kHz	1
#define HOUND_IFBW_120kHz	2
#define HOUND_IFBW_60kHz	4
#define HOUND_IFBW_30kHz	8
#define HOUND_IFBW_15kHz	16

class CSettings
{
public:
	CSettings(void);
	~CSettings(void);

	double		m_startFreq;		//Values in Hz.  Start, stop, center and span must all be valid before beginning sweep
	double		m_stopFreq;			// Use 
	double		m_centerFreq;
	double		m_spanFreq;

	double		m_stepFreq;			//Default 1 MHz step (IGNORED IN API)
	double		m_stepAmpl;			//Amplitude step (IGNORED IN API)
	double		m_refLevel;			//Ref level in dBm.  Used to set gains 
	double		m_refLevelOffset;	// (IGNORED IN API)

	int			m_refUnitsmV;		// (IGNORED IN API)
	double		m_logDbDiv;			// (IGNORED IN API)
	int			m_attenIndex;		// in 5 dB steps.  0 = 0dB, 3 = 15 dB
	bool		m_scaleLin;			// (IGNORED IN API)
	int			m_AmplUnits;		// (IGNORED IN API)
	bool		m_signalTrackOn;	// (IGNORED IN API)
	int			m_vidAvg;			// (IGNORED IN API)
	bool		m_TrigVideo;		// Untested in API
	double		m_TrigPos;			// 0 to 1, fraction of sweep time.  Amplitude for TG sweep
	int 		m_CalOutSyncTrig;	// (IGNORED IN API)
	double		m_videoTriggerLevel;// Untested in API

	int			m_ZSMode;			//Zero span  / demod mode, 0=amplitude, 1=frequency, 2=phase

	int			m_RBWSetpoint;		//Resolution bandwidth setpoint.  
									// Values from 2 to 24, representing 5 MHz to 0.1 Hz, respectively.
									// An index of 14 = RBW of 100 Hz.
									// See GetRBWFromIndex for other values. 
	int			m_VBWSetpoint;		//Same values as m_RBWSetpoint

	int			m_VDMMA;			//Includes min/max/average
	int			m_VDMode;			//Includes log/power/voltage modes

	bool		m_UseExtRef;		// Untested in API
	bool		m_RBWIsAuto;		// Set to allow software to choose best RBW. 
	bool		m_VBWIsAuto;		// Set to allow software to choose best VBW. 
	int			m_SWPTMSetpoint;	// Set from 0-4 meaning "fast" to "slow", as a suggested sweep speed

	bool		m_maxHold;			// (IGNORED IN API)
	bool		m_suppressImage;	// Image suppression on or off.
	int			m_decimation;		// Automatically updated by API

	int			m_MarkerSelected;	// (IGNORED IN API)
	int			m_sweepMode;		//	Selected mode for sweeping.  See #defines
	double		m_sweepTime;		// Not yet implemented in API

	int			m_Averaging;		// Automatically updated by API
	int			m_DetectorPasses;	// Automatically updated by API
	int			m_SubTraceCount;	// Automatically updated by API
	int			m_FFTSize;			// Automatically updated by API
	int			m_traceSize;		// Automatically updated by API
	double		m_ExtMixerOffset;	// (IGNORED IN API)
	double		m_ZSFreqPeak;		// (IGNORED IN API)
	double		m_ZSSweepTime;		// Set to control sweep time in zero span.

	int			m_SweepsToDo;		// (IGNORED IN API)

	int			m_Overpowered;		// Not yet implemented in API

	int			m_PreampOn;			// Updated by CMySignalHound::PreamplifierOnOff

	int			m_PNStartDecade;	// Not yet implemented in API
	int			m_PNStopDecade;		// Not yet implemented in API

	int			m_iReserved1;
	double		m_dReserved1;

	// //Calculate FFT size, decimation, RBW,VBW,etc.
	void CalcSweepParams(void);
	int GetBWIDX(double bw);
	bool isEqual(CSettings * p1, CSettings * p2);
	bool SetStartStop(double fStart, double fStop);

};
