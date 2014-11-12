// Author: Justin Crooks
// Test Equipment Plus
// Date: July 14, 2011

#include "CUSBSA.h"
#include "SHLAPI.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;

// The constructor allocates memory for a 
//   structure representing the Signal Hound device.
CUSBSA::CUSBSA() :
	is_initialized(false), is_configured(false), trace_len(0),
	trace_ampl(0), trace_freq(0), last_decimation(1)
{
    struct_sz = SHAPI_GetStructSize();
    sh_struct = (unsigned char*)malloc(struct_sz * sizeof(double));
}

// The destructor closes handles and frees allocated memory..
CUSBSA::~CUSBSA()
{
	if(is_initialized)
		SHAPI_Close(sh_struct);

	free(sh_struct);
	
	if(trace_ampl != NULL)
	  {
		  free(trace_ampl);
		  free(trace_freq);
	  }
}

// The initialize function takes a cal table as an OPTIONAL parameter.  If the cal table
// is absent, the device's internal table is read.  Initializes and prepares device for use.
int CUSBSA::Initialize(unsigned char *pCalData)
{
	int err = 0;

    if(!pCalData)
		{
			err = SHAPI_Initialize(sh_struct);
			if(err == 0) 
				is_initialized = true;
			SHAPI_CopyCalTable(sh_struct, m_CalTable);
			return err;
		}
	
    memcpy(m_CalTable, pCalData, 4096);

    err = SHAPI_InitializeEx(sh_struct, pCalData);
    if(err == 0) 
		is_initialized = true;

    return err;	
}

// The configure function sets internal clocks and selects RF / IF paths and gains.
int CUSBSA::Configure(double attenVal, 
					  int mixerBand, 
					  int sensitivity, 
					  int decimation, 
					  int useIF2_9, 
					  int ADCclock)
{
    last_decimation = decimation;

    if(!is_configured)
		{
			is_configured = true;
			return SHAPI_Configure(sh_struct, attenVal, mixerBand, 
								   sensitivity, decimation, useIF2_9, ADCclock);
		}

    return SHAPI_ConfigureFast(sh_struct, attenVal, mixerBand, 
							   sensitivity, decimation, useIF2_9, ADCclock);
}

// The slow sweep function acquires a trace of data, typically less than 1 MHz span.
int CUSBSA::SlowSweep(double start_freq, double stop_freq, int fft_len, 
					  int avgCount, int image_reject)
{
    // If never initialized, do it now.
    if(!is_initialized)
        if(!Initialize()) 
			return -1;

    // If never configured, configure now.
    if(!is_configured)
		{
			int bandGuess = 0; 
			if(stop_freq >= 150.0e6) 
				bandGuess = 1;
			Configure(10.0, bandGuess);
		}

	int return_len;
    int min_len = SHAPI_GetSlowSweepCount(sh_struct, start_freq, 
										  stop_freq, fft_len) + 1;

    if(trace_len < min_len) // We need a bigger trace buffer
		{
			if(trace_ampl != NULL)
				{
					free(trace_ampl);
					free(trace_freq);
				}
			
			trace_ampl = (double*)malloc(min_len * sizeof(double));
			trace_freq = (double*)malloc(min_len * sizeof(double));
			trace_len = min_len;
		}
	
    SHAPI_GetSlowSweep(sh_struct, trace_ampl, start_freq, stop_freq,
					   &return_len, fft_len, avgCount, image_reject);

    for(int i = 0; i < return_len; i++)
        trace_freq[i] = start_freq + i * 486111.111 / fft_len / last_decimation;

    m_dCalcRBW = 1.6384e6 / fft_len / last_decimation;

    return return_len;
}

// The fast sweep function acquires a trace of data
//   typically multiples of 200 KHz.
int CUSBSA::FastSweep(double start, double stop, int fft_len, int image_reject)
{
    int return_len = 0;
    int min_len = SHAPI_GetFastSweepCount(start, stop, fft_len) + 1;

    m_dCalcRBW = 1.6384e6 / fft_len;

    if(fft_len == 1) 
		m_dCalcRBW = 250.0e3;

    if(trace_len < min_len) // We need a bigger trace buffer
		{
			if(trace_ampl != NULL)
				{
					free(trace_ampl);
					free(trace_freq);
				}
						
			trace_ampl = (double*)malloc(min_len * sizeof(double));
			trace_freq = (double*)malloc(min_len * sizeof(double));
			trace_len = min_len;
		}
	
    // If never initialized, do it now.
    if(!is_initialized)
        if(!Initialize()) 
			return -1;
	
    // If never configured, configure now.
    if((!is_configured) || (last_decimation != 1))
		{
			int bandGuess = 0; 
			if(stop >= 150.0e6) 
				bandGuess = 1;
			Configure(10.0, bandGuess);
		}
	
    SHAPI_GetFastSweep(sh_struct, trace_ampl, start, stop,
					   &return_len, fft_len, image_reject);

    if(fft_len > 2)
		for(int i = 0; i < return_len; i++)
			trace_freq[i] = start + i * 400.0e3 / fft_len;
    else
		for(int i = 0; i < return_len; i++)
			trace_freq[i] = start + i * 2.0e5;

    return return_len;	
}

/* For reference only, very similar to FastSweep()
int CUSBSA::FastSweep_f(float startFreq, float stopFreq, int FFTSize, int imageHandling)
{
    int returnCount;
    int minSize = SHAPI_GetFastSweepCount(startFreq, stopFreq, FFTSize) + 1;

    m_dCalcRBW = 1.6384e6 / FFTSize;
    if(FFTSize == 1) 
		m_dCalcRBW = 250.0e3;

    if(trace_len < minSize) //We need a bigger trace buffer
    {
         if(fTraceAmpl != NULL)
        {
            free(fTraceAmpl);
            free(fTraceFreq);
        }

        int sz = minSize * sizeof(float);

        fTraceAmpl = (float *) malloc(sz);
        fTraceFreq = (float *) malloc(sz);
        trace_len = minSize;
   }


    // If never initialized, do it now.
    if(!is_initialized)
        if(!Initialize()) 
			return -1;

    // If never configured, configure now.
    if((is_configured) || (last_decimation != 1))
    {
        int bandGuess = 0; if(stopFreq >= 150.0e6) bandGuess = 1;
        Configure(10.0, bandGuess);
    }

    SHAPI_GetFastSweep_f(sh_struct, fTraceAmpl, startFreq, stopFreq,
						 &returnCount, FFTSize, imageHandling);
    if(FFTSize > 2)
		{
			for(int i = 0; i < returnCount; i++)
				fTraceFreq[i] = startFreq + i * 4.0e5 / FFTSize ;
		}
    else
		{
			for(int i = 0; i < returnCount; i++)
				fTraceFreq[i] = startFreq + i * 2.0e5;
		}

    return returnCount;
}
*/

// Turns the preamplifier on or off..
int CUSBSA::SetPreamp(int on_off)
{
    if(SHAPI_IsPreampAvailable(sh_struct))
        SHAPI_SetPreamp(sh_struct, on_off);
    else
        return 0;

    return on_off;
}

void CUSBSA::SetSyncTrig(int mode)
{
	SHAPI_SyncTriggerMode(sh_struct,mode);
}

int CUSBSA::External10MHz()
{
	return SHAPI_SelectExt10MHz(sh_struct);
}

int CUSBSA::Authenticate(int vendor_code)
{
	return SHAPI_Authenticate(sh_struct, vendor_code);
}

int CUSBSA::GetIQDataPacket(int *i_data, int *q_data, double *center_freq, int size)
{
	return SHAPI_GetIQDataPacket(sh_struct, i_data, q_data, center_freq, size);
}

int CUSBSA::SetupLO(double * center_freq, int mix_mode)
{
	return SHAPI_SetupLO(sh_struct, center_freq, mix_mode);
}

int CUSBSA::StartStreaming()
{
	return SHAPI_StartStreamingData(sh_struct);
}

int CUSBSA::StopStreaming()
{
	return SHAPI_StopStreamingData(sh_struct);
}

int CUSBSA::GetStreamingPacket(int *i_data, int *q_data)
{
	return SHAPI_GetStreamingPacket(sh_struct, i_data, q_data);
}

double CUSBSA::GetPhaseStep()
{
	return SHAPI_GetPhaseStep(sh_struct);
}

int CUSBSA::RunMeasurementReceiver()
{
	return SHAPI_RunMeasurementReceiver(sh_struct, &m_MeasRcvr);
}
