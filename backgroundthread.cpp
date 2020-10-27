#include "backgroundthread.h"

backgroundthread::backgroundthread(QStringList &Ips, XRFDeviceInfo *dptr){

    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD( 2, 2 );
    int iResult = WSAStartup( wVersionRequested, &wsaData);
    if (iResult != 0) {
        emit CommunicationError(WSA_ERROR, 0x00);
        IsErrors = true;
    }
    IPList = Ips;
    Dpp_sockets.resize(IPList.size());
    ICON_sockets.resize(IPList.size());
    unsigned long nonblock = 1;
    for(auto &dpp_sock : Dpp_sockets){
        dpp_sock = socket(PF_INET, SOCK_STREAM, 0);
        ioctlsocket(dpp_sock, FIONBIO, &nonblock);
    }
    for(auto &icon_sock : ICON_sockets){
        icon_sock = socket(PF_INET, SOCK_STREAM, 0);
        ioctlsocket(icon_sock, FIONBIO, &nonblock);
    }
    mode = 0;
    Stop = false;
    GlobalStop = false;
    pDevicesInformation = dptr;
}
backgroundthread::~backgroundthread(){
   WSACleanup();
   pDevicesInformation = nullptr;
}


void backgroundthread::PrepareToMeasure(){
    for(int nip = 0; nip < IPList.size(); ++ nip){
         if(OnboardStatusDP5[nip] && OnboardStatusICON[nip] && OnboardStatusMOXA[nip]){
                long PLen;
                CfgOptions.DppType = DP5Stat.m_DP5_Status.DEVICE_ID+5;
                CfgOptions.HwCfgDP5Out = "PRET=60";//SpecTime;
                CfgOptions.PC5_PRESENT = DP5Stat.m_DP5_Status.PC5_PRESENT;
                CfgOptions.SendCoarseFineGain = false;
                SndCmd.DP5_CMD_Config(DP5Proto.BufferOUT, XMTPT_SEND_CONFIG_PACKET_EX, CfgOptions);
                PLen = (DP5Proto.BufferOUT[4] * 256) + DP5Proto.BufferOUT[5] + 8;


                if(Send_Packet(Dpp_sockets[nip],PLen, true)){
                       SndCmd.DP5_CMD(DP5Proto.BufferOUT, XMTPT_DISABLE_MCA_MCS);
                       PLen = (DP5Proto.BufferOUT[4] * 256) + DP5Proto.BufferOUT[5] + 8;

                       if(Send_Packet(Dpp_sockets[nip],PLen, true)){
                           SndCmd.DP5_CMD(DP5Proto.BufferOUT,XMTPT_SEND_CLEAR_SPECTRUM_STATUS);
                           PLen = (DP5Proto.BufferOUT[4] * 256) + DP5Proto.BufferOUT[5] + 8;


                           if(Send_Packet(Dpp_sockets[nip],PLen, true)){
                               SndCmd.DP5_CMD(DP5Proto.BufferOUT, XMTPT_ENABLE_MCA_MCS);
                               PLen = (DP5Proto.BufferOUT[4] * 256) + DP5Proto.BufferOUT[5] + 8;



                               if(Send_Packet(Dpp_sockets[nip],PLen, true)){
                                   SndCmd.DP5_CMD(DP5Proto.BufferOUT, XMTPT_SEND_SPECTRUM_STATUS_AND_CLEAR);
                                   PLen = (DP5Proto.BufferOUT[4] * 256) + DP5Proto.BufferOUT[5] + 8;


                                   if(Send_Packet(Dpp_sockets[nip],PLen, true)){
                                       emit DeviceStatus(DP5_ONLINE, nip);
                                   }
                                   else{
                                      OnboardStatusDP5[nip] = 0;
                                      emit DeviceStatus(DP5_OFFLINE, nip);
                                      continue;
                                   }

                               }
                               else{
                                   OnboardStatusDP5[nip] = 0;
                                   emit DeviceStatus(DP5_OFFLINE, nip);
                                   continue;
                               }


                           }
                           else{
                               OnboardStatusDP5[nip] = 0;
                               emit DeviceStatus(DP5_OFFLINE, nip);
                               continue;
                           }


                       }
                       else{
                           OnboardStatusDP5[nip] = 0;
                           emit DeviceStatus(DP5_OFFLINE, nip);
                           continue;
                       }
                }
                else{
                    OnboardStatusDP5[nip] = 0;
                    emit DeviceStatus(DP5_OFFLINE, nip);
                    continue;
                }

         }

     }
}

void backgroundthread::TestConnections(bool CurrentMode){ // False - Main , True - Prepare
     // ICON TESTING
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    for(int nip = 0; nip < IPList.size(); ++ nip){

         struct addrinfo *bind_address;
         if(getaddrinfo(IPList[nip].toStdString().c_str(), "4002", &hints, &bind_address)){
             emit CommunicationError(GETADDRINFO_ERROR, nip);
             freeaddrinfo(bind_address);
             continue;
         }


         fd_set set_sockets;
         FD_ZERO(&set_sockets);
         FD_SET(ICON_sockets[nip], &set_sockets);


         if(::connect(ICON_sockets[nip], bind_address->ai_addr, bind_address->ai_addrlen)){
                 select(ICON_sockets[nip]+1, 0, &set_sockets, 0, &timeout);
                 if (FD_ISSET(ICON_sockets[nip], &set_sockets)) {
                           emit DeviceStatus(MOXA_ONLINE, nip);
                           OnboardStatusMOXA[nip] = 1;

                     // ICON STATUS //
                     send(ICON_sockets[nip], TakeStatus, sizeof(TakeStatus), 0);
                     Sleep(500);
                     recv(ICON_sockets[nip], Con_answer, sizeof(Con_answer), 0);
                     pDevicesInformation[nip].ICPCONDevice.ProcessOutputsState();

                     if(Con_answer[0] == 62)
                     {
                         OnboardStatusICON[nip] = 1;
                         UpdateICPCONInfo(nip); // UPDATE ICPCON INFORMATION
                         pDevicesInformation[nip].ICPCONDevice.ProcessOutputsState();
                         emit DeviceStatus(ICPCON_ONLINE, nip);
                         for(int i = 0; i < 6; ++ i ) pDevicesInformation[nip].ICPCONDevice.Con_answer[i] = 0;
                      }
                     else
                     {
                         OnboardStatusICON[nip] = 0;
                         emit DeviceStatus(ICPCON_OFFLINE, nip);
                     }
                     // ICON STATUS //


                 }
                 else{
                     OnboardStatusMOXA[nip] = 0;
                     emit DeviceStatus(MOXA_OFFLINE, nip);
                     OnboardStatusICON[nip] = 0;
                     emit DeviceStatus(ICPCON_OFFLINE, nip);
                 }
         }

      freeaddrinfo(bind_address);
    }// ICON TESTING


    // DPP TESTING
    for(int nip = 0; nip < IPList.size(); ++ nip){
        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        struct timeval timeout;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
         struct addrinfo *bind_address;

         if(getaddrinfo(IPList[nip].toStdString().c_str(), "4001", &hints, &bind_address)){
             emit CommunicationError(GETADDRINFO_ERROR, nip);
             freeaddrinfo(bind_address);
             continue;
         }

         fd_set set_sockets;
         FD_ZERO(&set_sockets);
         FD_SET(Dpp_sockets[nip], &set_sockets);


         if(::connect(Dpp_sockets[nip], bind_address->ai_addr, bind_address->ai_addrlen)){
                 select(Dpp_sockets[nip]+1, 0, &set_sockets, 0, &timeout);
                 if (FD_ISSET(Dpp_sockets[nip], &set_sockets)) {
                           OnboardStatusMOXA[nip] = 1;
                           emit DeviceStatus(MOXA_ONLINE, nip);

                     // DPP GET STATUS CODE //
                           DP5Proto.PacketIn[0] = 0;
                           DP5Proto.PacketIn[1] = 0;
                           SndCmd.DP5_CMD( DP5Proto.BufferOUT, XMTPT_SEND_STATUS);
                           long PLen = (DP5Proto.BufferOUT[4] * 256) +  DP5Proto.BufferOUT[5] + 8;
                           if(Send_Packet(Dpp_sockets[nip],PLen, CurrentMode)){
                               emit DeviceStatus(DP5_ONLINE, nip);
                               OnboardStatusDP5[nip] = 1;
                               // PROCESS STATUS //
                                ParsePkt.ParsePacket( DP5Proto.PacketIn, &DP5Proto.PIN);
                                long idxStatus;
                                for(idxStatus = 0; idxStatus < 64; idxStatus++){
                                     DP5Stat.m_DP5_Status.RAW[idxStatus] =  DP5Proto.PIN.DATA[idxStatus];
                                }
                                DP5Stat.Process_Status(&DP5Stat.m_DP5_Status);
                                DppStatusString = DP5Stat.ShowStatusValueStrings(DP5Stat.m_DP5_Status);
                                QString Status = QString::fromStdString(DppStatusString);
                                emit StringStatusReceived(Status, nip);
                               // PROCESS STATUS //

                           }
                           else{
                               emit DeviceStatus(DP5_OFFLINE, nip);
                               OnboardStatusDP5[nip] = 0;
                           }
                     // DPP GET STATUS CODE //

                }
                 else{
                     OnboardStatusMOXA[nip] = 0;
                     emit DeviceStatus(MOXA_OFFLINE, nip);
                     OnboardStatusDP5[nip] = 0;
                     emit DeviceStatus(DP5_OFFLINE, nip);
                 }

     }

    } // DPP TESTING



}

void backgroundthread::AcquireSpectrum(){
    for(int nip = 0; nip < IPList.size(); ++ nip){

    }
}

void backgroundthread::run(){
    if(!IsErrors){

      while(!GlobalStop){

          if(mode){
               PrepareToMeasure();
               while(!Stop){
                   TestConnections(false);
                   // Take a Spectrum //


                   // Take a Spectrum //

               }

          }
          else{  // Test Connection
                  TestConnections(true);
          }

      }


    } // if Errors ?
return;
}


bool backgroundthread::Send_Packet(SOCKET Dpp_socket, long &plen, bool Prepare){
    unsigned long l = 0;
    struct timeval timeout;
    if (Prepare){
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
    }
    else {
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
    }
    fd_set set_sockets;
    FD_ZERO(&set_sockets);
    FD_SET(Dpp_socket, &set_sockets);
    int iResult = send(Dpp_socket, reinterpret_cast<char*>(DP5Proto.BufferOUT), plen, 0);
    if(select(Dpp_socket+1, 0, &set_sockets, 0, &timeout) < 0) return false;
    else{
        if (FD_ISSET(Dpp_socket, &set_sockets)){
            ioctlsocket(Dpp_socket, FIONREAD, &l);
            if(l > 0){
               iResult = recv(Dpp_socket,  reinterpret_cast<char*>(DP5Proto.PacketIn), l, 0);
               if(DP5Proto.PacketIn[0] == 245 && DP5Proto.PacketIn[1] == 250) return true;
               else return false;
            }
            else return false;
         }
    }
    return false;
}

bool backgroundthread::get_icon_channel_state(int channel){
    if(channel < 0 || channel > 7) return -1;

        if(channel < 4){
            std::bitset<4> conn;
            for(int i = 0; i < 16; ++i){
            if(Con_answer[4] == hexx[i]){
               conn = i;
               break;
            }
            if(i == 15)return 0;
            }
          std::bitset<4> shifted = 1<<channel;
          std::bitset<4> answer = conn & shifted;
          return answer.to_ulong();
        }
        else{
           std::bitset<4> conn;
           for(int i = 0; i < 16; ++i){
           if(Con_answer[3] == hexx[i]){
              conn = i;
              break;
           }
           if(i == 15)return 0;
           }
           channel = 4 - (8-channel);
           std::bitset<4> shifted = 1<<channel;
           std::bitset<4> answer = conn & shifted;
           return answer.to_ulong();
        }
}


void backgroundthread::UpdateICPCONInfo(int device_id){
   for(int i = 0; i < 6; ++i){
      this->pDevicesInformation[device_id].ICPCONDevice.Con_answer[i] = this->Con_answer[i];
   }

    for(int i = 0; i < 6; ++i){
        this->pDevicesInformation[device_id].ICPCONDevice.Inputs_State[i] = static_cast<int>(get_icon_channel_state(i));
    }
}
