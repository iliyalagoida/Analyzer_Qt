
#pragma once

#include "ParsePacket.h"
#include "DppConst.h"
#include <QMessageBox>
#include <string>
#include <cctype> // std::toupper, std::tolower

using namespace std;

#define LINE_MAX 256
#define DP4_PX4_OLD_CFG_SIZE 64
#define DP5_MAX_CFG_SIZE 512			/// 512 + 8 Bytes (2 SYNC,2 PID,2 LEN,2 CHKSUM)
#define Whitespace "\t\n\v\f\r\0x20"	/// $ = Chr$(0) + Chr$(9) + Chr$(10) + Chr$(11) + Chr$(12) + Chr$(13) + Chr$(32)


typedef struct _CONFIG_OPTIONS
{
    bool PC5_PRESENT;				/// if true pc5 commands are supported
    int DppType;					/// device type indicator
    string HwCfgDP5Out;				/// custom configuration string
    bool SendCoarseFineGain;		/// FALSE=Send GAIN, TRUE=Send GAIA,GAIF
    bool isDP5_RevDxGains;			/// use dp5 dx gains
    unsigned char DPP_ECO;			/// holds eco indicator

} CONFIG_OPTIONS;



/** CSendCommand prepares all command packets to be sent.
    Call CSendCommand::DP5_CMD, CSendCommand::DP5_CMD_Config or CSendCommand::DP5_CMD_Data
    along with the ::TRANSMIT_PACKET_TYPE and any additional data or options.

    A command array of 8-bit bytes is generated.

*/
class CSendCommand
{
public:
    CSendCommand(void);
    ~CSendCommand(void);
    /// Forces all characters to uppercase. (All DPP command must be in upper case.)
    string MakeUpper(string myString);
    /// Packet checksum test for commands with data.
    bool TestPacketCkSumOK(unsigned char Data[]);
    /// Creates a DPP command that does not require additional processing.
    bool DP5_CMD(unsigned char Buffer[], TRANSMIT_PACKET_TYPE XmtCmd);
    /// Creates a DPP command that requires configuration data options processing.
    bool DP5_CMD_Config(unsigned char Buffer[], TRANSMIT_PACKET_TYPE XmtCmd, CONFIG_OPTIONS CfgOptions);
    /// Creates a DPP command that requires data.
    bool DP5_CMD_Data(unsigned char Buffer[], TRANSMIT_PACKET_TYPE XmtCmd, unsigned char DataOut[]);
    /// Creates a packet output buffer from a command byte data array.
    bool POUT_Buffer(Packet_Out POUT, unsigned char Buffer[]);
    /// Removes Whitespace characters from a command string.
    string RemWhitespace(string cstrLine);
    /// Reads a DPP configuration from a file.
    string GetDP5CfgStr(string cstrFilename);
    /// Generates a configuration readback command from a list of all commands.
    string CreateFullReadBackCmd(bool PC5_PRESENT, int DppType);
    /// Remove a specified command from the command stream.
    string RemoveCmd(string cstrCmd, string cstrCfgData);
    /// Remove illegal commands from the command stream by device type.
    string RemoveCmdByDeviceType(string cstrCfgDataIn, bool PC5_PRESENT, int DppType);
    /// Force string to ASCII bytes.
    bool CopyAsciiData(unsigned char Data[], string cstrCfg, long lLen);
};
























