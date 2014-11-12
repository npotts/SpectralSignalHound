// Author: Justin Crooks
// Test Equipment Plus
// Date: July 14, 2011

// Revision History:
// 7/14/11 Created.

//This is a simple class to encapsulate the functionality of one Signal Hound USB-SA44B

#ifndef CUSBSA_H
#define CUSBSA_H
#include <iostream>

struct MEAS_RCVR_STRUCT
{
	// *** INPUTS ***
	double	RFFrequency;	//RF frequency center
	double	AudioLPFreq;	//Audio LowPass Cutoff
	double	AudioBPFreq;	//Audio BandPass Center
	int		UseLPF;
	int		UseBPF;

	// *** OUTPUTS ***
	double	RFCounter;		//RF frequency count out
	double	AMAudioFreq;	//AF frequency count out after AM demod
	double	FMAudioFreq;	//AF frequency count out after FM demod
	double	RFAmplitude;	//dBFS

	double	FMPeakPlus;
	double	FMPeakMinus;
	double	FMRMS;

	double	AMPeakPlus;
	double	AMPeakMinus;
	double	AMRMS;

};

class CUSBSA
{
    public:
        CUSBSA();
        virtual ~CUSBSA();
        int Initialize(unsigned char * pCalData = NULL);
        int Configure(double attenVal=10.0, int mixerBand=0, int sensitivity=0, int decimation=1, int useIF2_9=0, int ADCclock=0);
        int SlowSweep(double startFreq, double stopFreq, int FFTSize=1024, int avgCount=16, int imageHandling=0);
        int FastSweep(double startFreq, double stopFreq, int FFTSize=16, int imageHandling=0);
        int SetPreamp(int onoff);
        void SetSyncTrig(int mode);
        int External10MHz();
        int Authenticate(int vendorcode);
        int GetIQDataPacket(int * pIData, int * pQData, double *centerFreq, int size);
        int SetupLO(double * centerFreq, int mixMode);
        int StartStreaming();
        int StopStreaming();
        int GetStreamingPacket(int * pIData, int * pQData);
        double GetPhaseStep();
        double GetRBW();
        int RunMeasurementReceiver();


        double * dTraceAmpl;
        double * dTraceFreq;
        int m_iTraceSize;
        double m_dCalcRBW;

    public:
        int m_iStructSize;
        bool m_bIsInitialized;
        bool m_bIsConfigured;
        int m_lastDecimation;
        unsigned char * m_pvHoundStruct;
        unsigned char m_CalTable[4096];
        MEAS_RCVR_STRUCT m_MeasRcvr;

    protected:
    private:
};



#endif // CUSBSA_H
