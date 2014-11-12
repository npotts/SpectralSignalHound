// Author: Justin Crooks
// Test Equipment Plus
// Date: July 14, 2011

// This is a simple class to encapsulate the 
//   functionality of one Signal Hound USB-SA44B

#ifndef CUSBSA_H
#define CUSBSA_H
#include <iostream>

struct MEAS_RCVR_STRUCT {
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

class CUSBSA {
 public:
	CUSBSA();
	~CUSBSA();
	
	int Initialize(unsigned char *pCalData = 0);
	int Configure(double attenVal = 10.0, int mixerBand = 0, int sensitivity = 0, 
				  int decimation = 1, int useIF2_9 = 0, int ADCclock = 0);

	int SlowSweep(double start_freq, double stop_freq, int fft_len = 1024, 
				  int avgCount = 16, int image_reject = 0);
	int FastSweep(double start_freq, double stop_freq, int fft_len = 16, 
				  int image_reject = 0);
	//int FastSweep_f(float start, float stop, int fft_len = 16, int image_reject = 0);

	int SetPreamp(int on_off);
	void SetSyncTrig(int mode);
	int External10MHz();
	int Authenticate(int vendor_code);
	int GetIQDataPacket(int *i_data, int *q_data, double *center_freq, int size);
	int SetupLO(double *center_freq, int mix_mode);
	int StartStreaming();
	int StopStreaming();
	int GetStreamingPacket(int *i_data, int *q_data);
	double GetPhaseStep();
	double GetRBW();
	int RunMeasurementReceiver();
	
	bool IsInitialized() { return is_initialized; }

	double *trace_ampl;
	double *trace_freq;

	double m_dCalcRBW;
	
 public:
	int struct_sz;
	int last_decimation;

	unsigned char *sh_struct;
	unsigned char m_CalTable[4096];
	MEAS_RCVR_STRUCT m_MeasRcvr;
	
 protected:
 private:
	int trace_len; // Allocated Length of trace buffers

	bool is_initialized;
	bool is_configured;
};

#endif // CUSBSA_H
