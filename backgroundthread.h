#ifndef BACKGROUNDTHREAD_H
#define BACKGROUNDTHREAD_H
#include <QMainWindow>
#include <QThread>
#include <elements.h>
#include <cstdlib>
#include <bitset>
#include "devices.h"



# define ICPCON_ONLINE (0x00)
# define ICPCON_OFFLINE (0x01)

# define MOXA_ONLINE (0x02)
# define MOXA_OFFLINE (0x03)

# define DP5_ONLINE (0x04)
# define DP5_OFFLINE (0x05)

# define WSA_ERROR ( 0x06 )
# define GETADDRINFO_ERROR ( 0x07 )




class backgroundthread : public QThread
{
    Q_OBJECT
public:
    backgroundthread(QStringList &Ips, XRFDeviceInfo * dptr = nullptr);
    ~backgroundthread();
    void run();
    friend class MainWindow;

private:
    XRFDeviceInfo *pDevicesInformation;
    char hexx[16] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70};
    string DppStatusString = " DP5 OFFLINE ";
    CParsePacket ParsePkt;
    CDP5Protocol DP5Proto;
    CSendCommand SndCmd;
    CDP5Status DP5Stat;
    CONFIG_OPTIONS CfgOptions;
    bool Send_Packet(SOCKET Dpp_socket, long &plen, bool Prepare);
    void PrepareToMeasure();
    void AcquireSpectrum();
    void UpdateICPCONInfo(int device_id);
    std::vector<bool> OnboardStatusMOXA;
    std::vector<bool> OnboardStatusICON;
    std::vector<bool> OnboardStatusDP5;
    std::vector<bool> AcquireRequestReceived;
    char TakeStatus[4] = { 64, 48, 49, 13 }; // @01 + "\n" - пакет запроса статуса с асипикона
    char Con_answer[6] = { 0 }; // массив для хранения ответа статуса
    char ICPCONBUFFER[6] = {0};
    QStringList IPList;
    std::vector<SOCKET> Dpp_sockets, ICON_sockets;
    void TestConnections(bool CurrentMode);
    bool IsErrors = false;
    std::atomic<int> mode;
    std::atomic<bool> Stop, GlobalStop;
    bool get_icon_channel_state(int channel);

signals:
    void CommunicationError(int code, int device_id);
    void DeviceStatus(int status, int device_id);
    void StringStatusReceived(QString status, int device_id);
};

#endif // BACKGROUNDTHREAD_H
