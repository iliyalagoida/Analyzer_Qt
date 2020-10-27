#ifndef DEVICES_H
#define DEVICES_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <vector>
#include "sendcommand.h"




//char TakeStatus[4] = { 64, 48, 49, 13 }; // @01 + "\n" - пакет запроса статуса с асипикона
struct ICPCON{
    char hexx[16] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70};
    int Inputs_State[8] = {0};
    int Outputs_State[8] = {0};
    char Con_answer[6] = { 0 }; // массив для хранения ответа статуса
    char ICPCONBUFFER[6] = {64,48,49,48,48,13};
    void ProcessOutputsState();
    void ManageChannel(int channel, bool Enable);
    unsigned char TranslatefromASCII(unsigned char number);
    unsigned char TranslatetoASCII(unsigned char number);
};



struct XRFDeviceInfo{
    double DeadTime=0.0;
    QString DppStatusString = " DP5 OFFLINE ";
    int FastCount;
    int SlowCount;
    double AccumulationTime=0.0;
    double RealTime=0.0;
    double DET_TEMP;
    double FPGA_TEMP;
    int Accepted_Spectrum[1024] = {0};
    int Rejected_Spectrum[1024] = {0};
    void Clear(){
        DeadTime=0;
        FastCount=0;
        SlowCount=0;
        AccumulationTime=0;
        RealTime=0;
        DET_TEMP=0;
        FPGA_TEMP=0;
        for(int i = 0; i < 1024; ++i){
           Accepted_Spectrum[i] = 0;
           Rejected_Spectrum[i] = 0;
        }
    }
    XRFDeviceInfo(){
        for(int i = 0; i < 1024; ++i){
           Accepted_Spectrum[i] = 0;
           Rejected_Spectrum[i] = 0;
        }
    }
    struct ICPCON ICPCONDevice;
};



#endif // DEVICES_H
