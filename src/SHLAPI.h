// Author: Justin Crooks
// Test Equipment Plus
// Date: July 11, 2011

// Revision History:
// 7/11/11 Created from windows API.

//This is a simple listing of available functions.  The CUSBSA class will use these functions.
// See the Windows API for further documentation.
#ifndef SHLAPI_H_INCLUDED
#define SHLAPI_H_INCLUDED
extern "C" void SHAPI_Close(void * pV);
extern "C" int SHAPI_CheckFirmware(void * pV, char * pStr);
extern "C" float SHAPI_GetTemperature(void * pV);
extern "C" int SHAPI_GetStructSize();
extern "C" void SHAPI_SetPreamp(void * pV,int val);
extern "C" int SHAPI_IsPreampAvailable(void * pV);
extern "C" double SHAPI_GetFMFFTPK(void * pV);
extern "C" double SHAPI_GetAudioFFTSample(void * pV,int idx);
extern "C" unsigned int SHAPI_GetSerNum(void * pV);
extern "C" void SHAPI_SyncTriggerMode(void * pV,int mode);
extern "C" void SHAPI_CyclePowerOnExit(void * pV);
extern "C" double SHAPI_GetAMFFTPK(void * pV);
extern "C" void SHAPI_ActivateAudioFFT(void * pV);
extern "C" void SHAPI_DeactivateAudioFFT(void * pV);
extern "C" double SHAPI_GetRBW(int FFTSize, int decimation);
extern "C" double SHAPI_GetLastChannelPower(void * pV);
extern "C" double SHAPI_GetLastChannelFreq(void * pV);
extern "C" void SHAPI_SetOscRatio(void * pV,double ratio);
extern "C" int SHAPI_Initialize(void * pV);
extern "C" void SHAPI_WriteCalTable(void * pV,unsigned char *myTable);
extern "C" int SHAPI_CopyCalTable(void * pV,unsigned char * p4KTable);
extern "C" int SHAPI_InitializeEx(void * pV,unsigned char * p4KTable);
extern "C" int SHAPI_SetAttenuator(void * pV,double attenVal);
extern "C" int SHAPI_SelectExt10MHz(void * pV);
extern "C" int SHAPI_Configure(void * pV,double attenVal, int mixerBand, int sensitivity,
												   int decimation, int useIF2_9, int ADCclock);
extern "C" int SHAPI_ConfigureFast(void * pV,double attenVal, int mixerBand, int sensitivity,
												   int decimation, int useIF2_9, int ADCclock);
extern "C" int SHAPI_GetSlowSweepCount(void * pV,double startFreq, double stopFreq, int FFTSize);
extern "C" int SHAPI_GetSlowSweep(void * pV,double * dBArray, double startFreq, double stopFreq,
													  int * returnCount, int FFTSize,
													  int avgCount, int imageHandling);
extern "C" int SHAPI_GetFastSweepCount(double startFreq, double stopFreq, int FFTSize);
extern "C" int SHAPI_CyclePort(void * pV);
extern "C" int SHAPI_GetFastSweep(void * pV, double * dBArray, double startFreq, double stopFreq,
													  int * returnCount, int FFTSize, int imageHandling);
extern "C" int SHAPI_GetFastSweep_f(void * pV, float * dBArray, float startFreq, float stopFreq,
													  int * returnCount, int FFTSize, int imageHandling);
extern "C" int SHAPI_GetIQDataPacket(void * pV,int * pIData, int * pQData, double *centerFreq, int size);
extern "C" int SHAPI_Authenticate(void * pV,int vendorcode);
extern "C" int SHAPI_SetupLO(void * pV,double *centerFreq, int mixMode);
extern "C" int SHAPI_StartStreamingData(void * pV);
extern "C" int SHAPI_StopStreamingData(void * pV);
extern "C" int SHAPI_GetStreamingPacket(void * pV, int *bufI, int *bufQ);
extern "C" double SHAPI_GetPhaseStep(void * pV);
extern "C" int SHAPI_GetIntFFT(void * pV, int FFTSize, int *iBigI, int *iBigQ, double * dFFTOut);
extern "C" int SHAPI_SetupFastSweepLoop(void * pV,double startFreq, double stopFreq, int *returnCount, int MaxFFTSize, int imageHandling, int AvgCount);
extern "C" int SHAPI_GetFSLoopIQSize(double startFreq, double stopFreq, int MaxFFTSize);
extern "C" int SHAPI_GetFSLoopIQ(void * pV, int * pI1, int * pQ1, int * pI2, int * pQ2, double startFreq, double stopFreq, int MaxFFTSize, int imageHandling);
extern "C" int SHAPI_ProcessFSLoopData(void * pV, double * dBArray, int * pI1, int * pQ1, int * pI2, int * pQ2, double startFreq, double stopFreq,
												int *returnCount, int FFTSize,int MaxFFTSize, int imageHandling);
extern "C" double  SHAPI_GetChannelPower(void * pV, int * pI1, int * pQ1, int * pI2, int * pQ2, double startFreq, double stopFreq,
												double channelFreq, double channelBandwidth, int MaxFFT, int imageHandling);
extern "C" int  SHAPI_RunMeasurementReceiver(void * pV,void * LPStruct);



#endif // SHLAPI_H_INCLUDED
