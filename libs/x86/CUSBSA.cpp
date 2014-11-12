// Author: Justin Crooks
// Test Equipment Plus
// Date: July 14, 2011

// Revision History:
// 7/14/11 Created.

//This is a simple class to encapsulate the functionality of one Signal Hound USB-SA44B

#include "CUSBSA.h"
#include "SHLAPI.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;

// The constructor allocates memory for a structure representing the Signal Hound device.
CUSBSA::CUSBSA()
{
    //ctor

    m_iStructSize = SHAPI_GetStructSize();
    m_bIsInitialized = false;
    m_bIsConfigured = false;
    m_pvHoundStruct = new unsigned char[m_iStructSize];
    dTraceAmpl = NULL;
    dTraceFreq = NULL;
    m_iTraceSize=0;
    m_lastDecimation = 1;
}

// The destructor closes handles and frees allocated memory..
CUSBSA::~CUSBSA()
{
    //dtor
    if(m_bIsInitialized)
        SHAPI_Close(m_pvHoundStruct);
    delete m_pvHoundStruct;

    if(dTraceAmpl != NULL)
    {
       delete dTraceAmpl;
       delete dTraceFreq;

    }
 }

// The initialize function takes a cal table as an OPTIONAL parameter.  If the cal table
// is absent, the device's internal table is read.  Initializes and prepares device for use.
int CUSBSA::Initialize(unsigned char * pCalData)
{
    if(pCalData==NULL)
    {
        int errorval = SHAPI_Initialize(m_pvHoundStruct);
        if(errorval==0) m_bIsInitialized = true;
        SHAPI_CopyCalTable(m_pvHoundStruct,m_CalTable);
        return errorval;
    }

    memcpy(m_CalTable,pCalData,4096);
    int errorval = SHAPI_InitializeEx(m_pvHoundStruct,pCalData);
    if(errorval==0) m_bIsInitialized = true;
    return errorval;

}

// The configure function sets internal clocks and selects RF / IF paths and gains.
int CUSBSA::Configure(double attenVal, int mixerBand, int sensitivity, int decimation, int useIF2_9, int ADCclock)
{
    m_lastDecimation = decimation;
    if(!m_bIsConfigured)
    {
        m_bIsConfigured = true;
        return SHAPI_Configure(m_pvHoundStruct, attenVal, mixerBand, sensitivity, decimation, useIF2_9, ADCclock);
    }

    return SHAPI_ConfigureFast(m_pvHoundStruct, attenVal, mixerBand, sensitivity, decimation, useIF2_9, ADCclock);
}

// The slow sweep function acquires a trace of data, typically less than 1 MHz span.
int CUSBSA::SlowSweep(double startFreq, double stopFreq, int FFTSize, int avgCount, int imageHandling)
{
    // If never initialized, do it now.
    if(!m_bIsInitialized)
        if(Initialize()!=0) return -1;//Error not initialized

    // If never configured, configure now.
    if(!m_bIsConfigured)
    {
        int bandGuess = 0; if(stopFreq >= 150.0e6) bandGuess = 1;
        Configure(10.0, bandGuess);
    }

    int returnCount;
    int i;
    int minSize = SHAPI_GetSlowSweepCount(m_pvHoundStruct,startFreq,stopFreq,FFTSize) + 1;

    if(m_iTraceSize < minSize) //We need a bigger trace buffer
    {
         if(dTraceAmpl != NULL)
        {
            delete dTraceAmpl;
            delete dTraceFreq;
        }

        dTraceAmpl = new double[minSize];
        dTraceFreq = new double[minSize];
        m_iTraceSize = minSize;
   }


    SHAPI_GetSlowSweep(m_pvHoundStruct,dTraceAmpl,startFreq,stopFreq,&returnCount,FFTSize,avgCount,imageHandling);
//    if(returnCount > dTraceSize) cout << "size error" << dTraceSize << " < " << returnCount << endl;

    for(i=0; i<returnCount; i++)
        dTraceFreq[i] = startFreq + i * 486111.111 / FFTSize / m_lastDecimation;

    m_dCalcRBW = 1.6384e6 / FFTSize / m_lastDecimation;

    return returnCount;

}

// The fast sweep function acquires a trace of data, typically multiples of 200 KHz.
int CUSBSA::FastSweep(double startFreq, double stopFreq, int FFTSize, int imageHandling)
{
    int returnCount;
    int i;
    int minSize = SHAPI_GetFastSweepCount(startFreq,stopFreq,FFTSize) + 1;

    m_dCalcRBW = 1.6384e6 / FFTSize;
    if(FFTSize==1) m_dCalcRBW = 250.0e3;

    if(m_iTraceSize < minSize) //We need a bigger trace buffer
    {
        if(dTraceAmpl != NULL)
        {
            delete dTraceAmpl;
            delete dTraceFreq;
        }

        dTraceAmpl = new double[minSize];
        dTraceFreq = new double[minSize];
        m_iTraceSize = minSize;
    }

    // If never initialized, do it now.
    if(!m_bIsInitialized)
        if(Initialize()!=0) return -1;//Error not initialized

    // If never configured, configure now.
    if((!m_bIsConfigured) || (m_lastDecimation!=1))
    {
        int bandGuess = 0; if(stopFreq >= 150.0e6) bandGuess = 1;
        Configure(10.0, bandGuess);
    }

    SHAPI_GetFastSweep(m_pvHoundStruct,dTraceAmpl,startFreq,stopFreq,&returnCount,FFTSize,imageHandling);
    if(FFTSize>2)
    {
        for(i=0; i<returnCount; i++)
            dTraceFreq[i] = startFreq + i * 4.0e5 / FFTSize ;
    }
    else
    {
        for(i=0; i<returnCount; i++)
            dTraceFreq[i] = startFreq + i * 2.0e5;
    }
    return returnCount;

}

// Turns the preamplifier on or off..
int CUSBSA::SetPreamp(int onoff)
{
    if(SHAPI_IsPreampAvailable(m_pvHoundStruct))
        SHAPI_SetPreamp(m_pvHoundStruct,onoff);
    else
        return 0;
    return onoff;
}

void CUSBSA::SetSyncTrig(int mode)
{
     SHAPI_SyncTriggerMode(m_pvHoundStruct,mode);
}

int CUSBSA::External10MHz()
{
     return SHAPI_SelectExt10MHz(m_pvHoundStruct);
}

int CUSBSA::Authenticate(int vendorcode)
{
     return SHAPI_Authenticate(m_pvHoundStruct,vendorcode);
}

int CUSBSA::GetIQDataPacket(int * pIData, int * pQData, double *centerFreq, int size)
{
     return SHAPI_GetIQDataPacket(m_pvHoundStruct, pIData, pQData, centerFreq, size);
}

int CUSBSA::SetupLO(double * centerFreq, int mixMode)
{
     return SHAPI_SetupLO(m_pvHoundStruct,centerFreq, mixMode);
}

int CUSBSA::StartStreaming()
{
     return SHAPI_StartStreamingData(m_pvHoundStruct);
}

int CUSBSA::StopStreaming()
{
     return SHAPI_StopStreamingData(m_pvHoundStruct);
}

int CUSBSA::GetStreamingPacket(int * pIData, int * pQData)
{
     return SHAPI_GetStreamingPacket(m_pvHoundStruct, pIData, pQData);
}

double CUSBSA::GetPhaseStep()
{
     return SHAPI_GetPhaseStep(m_pvHoundStruct);
}

int CUSBSA::RunMeasurementReceiver()
{
     return SHAPI_RunMeasurementReceiver(m_pvHoundStruct,&m_MeasRcvr);
}
