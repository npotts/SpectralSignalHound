
/**

File: AudioStream.h
Copyright 2012 Test Equipment Plus
Author: Justin Crooks
E-Mail: justin@teplus.com
Description: Header file for CAudioStream Class
			which is used for demodulating audio.

Revision History:
12 JAN 2012		Justin Crooks		Created from Signal Hound code.  Removed Windows dependancies.

*/
#include <stdlib.h>

#pragma once


#define DEMOD_MODE_AM	0 //Amplitude modulation
#define DEMOD_MODE_FM	1 //Frequency modulation
#define DEMOD_MODE_USB	2 //Single sideband (upper sideband)
#define DEMOD_MODE_LSB	3 //Single sideband (lower sideband)
#define DEMOD_MODE_CW	4 //Continuous Wave / Morse Code


class CAudioStream
{
public:
	CAudioStream(void);
	~CAudioStream(void);

	int ReadAudioChunk(short * buffer);
	void StopAudio();
	void StartAudio();
	void * m_pvUSB;

	int * m_bufI;
	int * m_bufQ;
	int m_demodMode;
	int m_decimation;

	double m_AMAGC;
	double m_AGCcoeff;
	double m_deempL;//For de-emphasis
	double m_deempcoeff;
	double m_audioAvg;
	double m_demodAvg;
	double m_tonePhase;
	double m_phaseStep;
	double m_RSSIPower;
	double m_deemp;

	double m_lastPhase;
	double origdataR[8]; //For recursive filter
	double procdataR[8];
	double origdataI[8]; //For recursive filter
	double procdataI[8];
	double origdataQ[8]; //For recursive filter
	double procdataQ[8];
	double deempL;//For de-emphasis
	double deempcoeff;


	double AudioRecursiveFilter(double dataIn);
	double IRecursiveFilter(double dataIn);
	double QRecursiveFilter(double dataIn);

};
