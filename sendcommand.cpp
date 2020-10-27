
#include "SendCommand.h"
#include "stringex.h"

CSendCommand::CSendCommand(void)
{
}

CSendCommand::~CSendCommand(void)
{
}

string CSendCommand::MakeUpper(string StdString)
{
  const int length = (int)StdString.length();
  for(int i=0; i<length ; ++i)
  {
    StdString[i] = std::toupper(StdString[i]);
  }
  return StdString;
}

bool CSendCommand::TestPacketCkSumOK(unsigned char Data[])
{
    long idxBuffer;
    long CS;
    long PktLen;
    unsigned char CHKSUM_MSB;
    unsigned char CHKSUM_LSB;

    PktLen = (Data[4] * 256) + Data[5];
    CS = Data[0] + Data[1] + Data[2] + Data[3] + Data[4] + Data[5];
    if (PktLen > 0) {
        for (idxBuffer=0; idxBuffer<PktLen;idxBuffer++) {
            CS = CS + Data[idxBuffer + 6];
        }
    }
    CS = (CS ^ 0xFFFF) + 1;
    CHKSUM_MSB = (unsigned char)((CS & 0xFF00) / 256);	// calculated checksum
    CHKSUM_LSB = (unsigned char)(CS & 0xFF);
    if ((Data[PktLen + 6] == CHKSUM_MSB) && (Data[PktLen + 7] == (CS & 0xFF))) {
        return TRUE;
    } else {
        return FALSE;
    }
}

string CSendCommand::RemoveCmd(string strCmd, string strCfgData)
{
    int iStart,iEnd,iCmd;
    string strNew;

    strNew = "";
    if (strCfgData.length() < 7) { return strCfgData; }	// no data
    if (strCmd.length() != 4) {	return strCfgData; }		// bad command
    iCmd = (int)strCfgData.find(strCmd,0);
    if (iCmd == -1) { return strCfgData; }						// cmd not found
    iStart = iCmd;
    iEnd = (int)strCfgData.find(";",iCmd);
    if (iEnd == -1) { return strCfgData; }						// end not found
    if (iEnd <= iStart) { return strCfgData; }					// unknown error
    strNew = strCfgData.substr(0,iStart) + strCfgData.substr(iEnd+1);
    return strNew;
}

////removes selected command by dpp device type
string CSendCommand::RemoveCmdByDeviceType(string strCfgDataIn, bool PC5_PRESENT, int DppType)
{
    string strCfgData;
    bool isHVSE;
    bool isPAPS;
    bool isTECS;
    bool isVOLU;
    bool isCON1;
    bool isCON2;
    bool isINOF;
    bool isBOOT;
    bool isGATE;
    bool isPAPZ;

    strCfgData = strCfgDataIn;
    isHVSE = (((DppType !=  devtypePX5) && PC5_PRESENT) || DppType == devtypePX5);
    isPAPS = (DppType !=  devtypeDP5G);
    isTECS = (((DppType == devtypeDP5) && PC5_PRESENT) || (DppType != devtypeDP5G));
    isVOLU = (DppType == devtypePX5);
    isCON1 = (DppType != devtypeDP5);
    isCON2 = (DppType != devtypeDP5);
    isINOF = (DppType != devtypeDP5G);
    isBOOT = (DppType == devtypeDP5);
    isGATE = (DppType == devtypeDP5);
    isPAPZ = (DppType == devtypePX5);
    if (!isHVSE) { strCfgData = RemoveCmd("HVSE", strCfgData); }  //High Voltage Bias
    if (!isPAPS) { strCfgData = RemoveCmd("PAPS", strCfgData); }  //Preamp Voltage
    if (!isTECS) { strCfgData = RemoveCmd("TECS", strCfgData); }  //Cooler Temperature
    if (!isVOLU) { strCfgData = RemoveCmd("VOLU", strCfgData); }  //px5 speaker
    if (!isCON1) { strCfgData = RemoveCmd("CON1", strCfgData); }  //connector 1
    if (!isCON2) { strCfgData = RemoveCmd("CON2", strCfgData); }  //connector 2
    if (!isINOF) { strCfgData = RemoveCmd("INOF", strCfgData); }  //input offset
    if (!isBOOT) { strCfgData = RemoveCmd("BOOT", strCfgData); }  //PC5 On At StartUp
    if (!isGATE) { strCfgData = RemoveCmd("GATE", strCfgData); }  //Gate input
    if (!isPAPZ) { strCfgData = RemoveCmd("PAPZ", strCfgData); }  //Pole-Zero
    return strCfgData;
}

//Extend Packet_Out to include entire message
//include send packet data finishing into dp5_cmd
//remove control and interface objects
//move send packet to separate function

bool CSendCommand::DP5_CMD(unsigned char Buffer[], TRANSMIT_PACKET_TYPE XmtCmd)
{
    bool bCmdFound;
    string D;
    Packet_Out POUT;

    bCmdFound = TRUE;
    POUT.LEN = 0;
    string strCfg;
    long lLen;
    //long idxData;

    switch (XmtCmd) {
        ////REQUEST_PACKETS_TO_DP5
        case XMTPT_SEND_SPECTRUM_STATUS_AND_CLEAR:
            POUT.PID1 = 0x02;
            POUT.PID2 = 0x04;   // send spectrum & status & and clear
                break;
        case XMTPT_SEND_STATUS:
            POUT.PID1 = PID1_REQ_STATUS;
            POUT.PID2 = PID2_SEND_DP4_STYLE_STATUS;   // send status only
            break;
        //case XMTPT_SEND_SPECTRUM:
            //break;
        //case XMTPT_SEND_CLEAR_SPECTRUM:
            //break;
        case XMTPT_SEND_SPECTRUM_STATUS:
            POUT.PID1 = PID1_REQ_SPECTRUM;
            POUT.PID2 = PID2_SEND_SPECTRUM_STATUS;   // send spectrum & status
            break;
        case XMTPT_SEND_CLEAR_SPECTRUM_STATUS:
            POUT.PID1 = PID1_REQ_SPECTRUM;
            POUT.PID2 = PID2_SEND_CLEAR_SPECTRUM_STATUS;   // send & clear spectrum & status
            break;
        //case XMTPT_BUFFER_SPECTRUM:
            //break;
        //case XMTPT_BUFFER_CLEAR_SPECTRUM:
            //break;
        //case XMTPT_SEND_BUFFER:
            //break;
        //case XMTPT_SEND_DP4_STYLE_STATUS:
            //break;
        //case XMTPT_SEND_CONFIG:
            //break;
        case XMTPT_SEND_SCOPE_DATA:
            POUT.PID1 = PID1_REQ_SCOPE_MISC;
            POUT.PID2 = PID2_SEND_SCOPE_DATA;
            break;
        case XMTPT_SEND_512_BYTE_MISC_DATA:
            POUT.PID1 = PID1_REQ_SCOPE_MISC;
            POUT.PID2 = PID2_SEND_512_BYTE_MISC_DATA; // request misc data
            break;
        case XMTPT_SEND_SCOPE_DATA_REARM:
            POUT.PID1 = PID1_REQ_SCOPE_MISC;
            POUT.PID2 = PID2_SEND_SCOPE_DATA_REARM;
            break;
        //case XMTPT_SEND_ETHERNET_SETTINGS:
            //break;
        case XMTPT_SEND_DIAGNOSTIC_DATA:
            POUT.PID1 = PID1_REQ_SCOPE_MISC;
            POUT.PID2 = PID2_SEND_DIAGNOSTIC_DATA;   // Request Diagnostic Packet
            POUT.LEN = 0;
            break;
        case XMTPT_SEND_NETFINDER_PACKET:
            POUT.PID1 = PID1_REQ_SCOPE_MISC;
            POUT.PID2 = PID2_SEND_NETFINDER_READBACK;   // Request NetFinder Packet
            POUT.LEN = 0;
            break;
        //case XMTPT_SEND_HARDWARE_DESCRIPTION:
            //break;
        //case XMTPT_SEND_SCA:
            //break;
        //case XMTPT_LATCH_SEND_SCA:
            //break;
        //case XMTPT_LATCH_CLEAR_SEND_SCA:
            //break;
        //case XMTPT_SEND_ROI_OR_FIXED_BLOCK:
            //break;
   //     case XMTPT_PX4_STYLE_CONFIG_PACKET:
            //if (GetPX4RawCfg(POUT.DATA)) {
            //	POUT.PID1 = PID1_REQ_CONFIG;
            //	POUT.PID2 = PID2_PX4_STYLE_CONFIG_PACKET;   // PX4-style config packet
            //	POUT.LEN = DP4_PX4_OLD_CFG_SIZE;
            //}
            //break;
        case XMTPT_READ_CONFIG_PACKET:
            strCfg = "";
            strCfg += "CLCK=?;"; // FPGA clock
            strCfg += "TPEA=?;"; // peak time
            strCfg += "GAIN=?;"; // gain
            strCfg += "MCAS=?;"; // mca mode
            strCfg += "MCAC=?;"; // channels
            strCfg += "INOF=?;"; // osc. Input offset
            strCfg += "THSL=?;"; // LLD thresh
            strCfg += "THFA=?;"; // fast thresh
            strCfg += "DACO=?;"; // osc. DAC output
            strCfg += "DACF=?;"; // osc. DAC offset
            strCfg += "AUO1=?;"; // osc. AUX_OUT1
            strCfg += "PRET=?;"; // preset actual time
            strCfg += "PRER=?;"; // preset real time
            strCfg += "PREC=?;"; // preset count
            strCfg += "SCOE=?;"; // osc. Scope trigger edge
            strCfg += "SCOT=?;"; // osc. Scope trigger position
            strCfg += "SCOG=?;"; // osc. Scope gain

            lLen = (long)strCfg.length();
            if (lLen > 0) {
                strCfg = MakeUpper(strCfg);
                CopyAsciiData(POUT.DATA, strCfg, lLen);
            }
            POUT.PID1 = PID1_REQ_CONFIG;
            POUT.PID2 = PID2_CONFIG_READBACK_PACKET;   // read config packet
            POUT.LEN = (USHORT)lLen;
            break;
        case XMTPT_SCA_READ_CONFIG_PACKET:
            strCfg = "";
            strCfg = "SCAW=?;";
            strCfg +="SCAI=1;SCAL=?;SCAH=?;SCAO=?;";
            strCfg +="SCAI=2;SCAL=?;SCAH=?;SCAO=?;";
            strCfg +="SCAI=3;SCAL=?;SCAH=?;SCAO=?;";
            strCfg +="SCAI=4;SCAL=?;SCAH=?;SCAO=?;";
            strCfg +="SCAI=5;SCAL=?;SCAH=?;SCAO=?;";
            strCfg +="SCAI=6;SCAL=?;SCAH=?;SCAO=?;";
            strCfg +="SCAI=7;SCAL=?;SCAH=?;SCAO=?;";
            strCfg +="SCAI=8;SCAL=?;SCAH=?;SCAO=?;";
            lLen = (long)strCfg.length();
            if (lLen > 0) {
                strCfg = MakeUpper(strCfg);
                CopyAsciiData(POUT.DATA, strCfg, lLen);
            }
            POUT.PID1 = PID1_REQ_CONFIG;
            POUT.PID2 = PID2_CONFIG_READBACK_PACKET;   // read config packet
            POUT.LEN = (USHORT)lLen;
            break;
         case XMTPT_ERASE_FPGA_IMAGE:
            POUT.PID1 = PID1_REQ_FPGA_UC;
            POUT.PID2 = PID2_ERASE_FPGA_IMAGE;
            POUT.LEN = 2;
            POUT.DATA[0] = 0x12;
            POUT.DATA[1] = 0x34;
            break;
        //case XMTPT_UPLOAD_PACKET_FPGA:
            //break;
        //case XMTPT_REINITIALIZE_FPGA:
            //break;
        //case XMTPT_ERASE_UC_IMAGE_0:
            //break;
        case XMTPT_ERASE_UC_IMAGE_1:
            POUT.PID1 = PID1_REQ_FPGA_UC;
            POUT.PID2 = PID2_ERASE_UC_IMAGE_1;   // erase image #1 (sector 5)
            POUT.LEN = 2;
            POUT.DATA[0] = 0x12;
            POUT.DATA[1] = 0x34;
            break;
        //case XMTPT_ERASE_UC_IMAGE_2:
            //break;
        //case XMTPT_UPLOAD_PACKET_UC:
            //break;
        //case XMTPT_SWITCH_TO_UC_IMAGE_0:
            //break;
        case XMTPT_SWITCH_TO_UC_IMAGE_1:
            POUT.PID1 = PID1_REQ_FPGA_UC;
            POUT.PID2 = PID2_SWITCH_TO_UC_IMAGE_1;   // switch to uC image #1
            POUT.LEN = 2;
            POUT.DATA[0] = 0xA5; // uC FLASH unlock keys
            POUT.DATA[1] = 0xF1;
            break;
        //case XMTPT_SWITCH_TO_UC_IMAGE_2:
            //break;
        //case XMTPT_UC_FPGA_CHECKSUMS:
            //break;
        ////VENDOR_REQUESTS_TO_DP5
        //case XMTPT_CLEAR_SPECTRUM_BUFFER_A:
            //break;
        case XMTPT_ENABLE_MCA_MCS:
            POUT.PID1 = PID1_VENDOR_REQ;
            POUT.PID2 = PID2_ENABLE_MCA_MCS;
            POUT.LEN = 0;
            break;
        case XMTPT_DISABLE_MCA_MCS:
            POUT.PID1 = PID1_VENDOR_REQ;
            POUT.PID2 = PID2_DISABLE_MCA_MCS;
            POUT.LEN = 0;
            break;
        case XMTPT_ARM_DIGITAL_OSCILLOSCOPE:
            POUT.PID1 = PID1_VENDOR_REQ;
            POUT.PID2 = PID2_ARM_DIGITAL_OSCILLOSCOPE;   // arm trigger
            break;
        //case XMTPT_AUTOSET_INPUT_OFFSET:
            //break;
        case XMTPT_AUTOSET_FAST_THRESHOLD:
            POUT.PID1 = PID1_VENDOR_REQ;
            POUT.PID2 = PID2_AUTOSET_FAST_THRESHOLD;
            POUT.LEN = 0;
            break;
        //case XMTPT_READ_IO3_0:
            //break;
        //case XMTPT_WRITE_IO3_0:
            //break;
        //case XMTPT_SET_DCAL:
            //break;
        //case XMTPT_SET_PZ_CORRECTION_UC_TEMP_CAL:
            //break;
        //case XMTPT_SET_PZ_CORRECTION_UC_TEMP_CAL:
            //break;
        //case XMTPT_SET_BOOT_FLAGS:
            //break;
        //case XMTPT_SET_HV_DP4_EMULATION:
            //break;
        //case XMTPT_SET_TEC_DP4_EMULATION:
            //break;
        //case XMTPT_SET_INPUT_OFFSET_DP4_EMULATION:
            //break;
        //case XMTPT_SET_ADC_CAL_GAIN_OFFSET:
            //break;
        //case XMTPT_SET_SPECTRUM_OFFSET:
            //break;
        //case XMTPT_REQ_SCOPE_DATA_MISC_DATA_SCA_PACKETS:
            //break;
        //case XMTPT_SET_SERIAL_NUMBER:
            //break;
        //case XMTPT_CLEAR_GP_COUNTER:
            //break;
        //case XMTPT_SWITCH_SUPPLIES:
            //break;
        //case XMTPT_SEND_TEST_PACKET:
            //break;
        case XMTPT_REQ_ACK_PACKET:
            POUT.PID1 = PID1_COMM_TEST;
            POUT.PID2 = PID2_ACK_OK;
            POUT.LEN = 0;
            break;

        default:
            bCmdFound = FALSE;
            break;
    }
    if (bCmdFound) {
        if (! POUT_Buffer(POUT, Buffer)) {
            bCmdFound = FALSE;
        }
    }
    return bCmdFound;
}

//DP5_CMD_Config is for: (requires pc5 and device info)
//		XMTPT_SEND_CONFIG_PACKET_TO_HW
//		XMTPT_SEND_CONFIG_PACKET_EX
//		XMTPT_FULL_READ_CONFIG_PACKET
bool CSendCommand::DP5_CMD_Config(unsigned char Buffer[], TRANSMIT_PACKET_TYPE XmtCmd, CONFIG_OPTIONS CfgOptions)
{
    bool bCmdFound;
    string D;
    Packet_Out POUT;
    bCmdFound = TRUE;
    POUT.LEN = 0;
    string strCfg;
    long lLen;

    switch (XmtCmd) {
        case XMTPT_SEND_CONFIG_PACKET_TO_HW:
            // CONFIG_OPTIONS Needed:
            //		CfgOptions.HwCfgDP5Out
            //		CfgOptions.SendCoarseFineGain
            //		CfgOptions.PC5_PRESENT
            //		CfgOptions.DppType
            strCfg = "";
            strCfg = CfgOptions.HwCfgDP5Out;
            CfgOptions.SendCoarseFineGain = false;
            if (CfgOptions.SendCoarseFineGain) {
                strCfg = RemoveCmd("GAIN",strCfg);
            } else {
                strCfg = RemoveCmd("GAIA",strCfg);
                strCfg = RemoveCmd("GAIF",strCfg);
            }
            strCfg = RemoveCmdByDeviceType(strCfg, CfgOptions.PC5_PRESENT, CfgOptions.DppType);
            lLen = (long)strCfg.length();
            if (lLen > 0) {
                strCfg = MakeUpper(strCfg);
                CopyAsciiData(POUT.DATA, strCfg, lLen);
            }
            POUT.PID1 = PID1_REQ_CONFIG;  // PID1_REQ_CONFIG = 0x20,
            POUT.PID2 = PID2_TEXT_CONFIG_PACKET;   // text config packet PID2_TEXT_CONFIG_PACKET = 0x02,
            POUT.LEN = (USHORT)lLen;
            break;
        case XMTPT_SEND_CONFIG_PACKET_EX:			// bypass any filters
            // CONFIG_OPTIONS Needed:
            // CfgOptions.HwCfgDP5Out
            strCfg = "";
            strCfg = CfgOptions.HwCfgDP5Out;
            lLen = (long)strCfg.length();
            if (lLen > 0) {
                strCfg = MakeUpper(strCfg);
                CopyAsciiData(POUT.DATA, strCfg, lLen);
            }
            POUT.PID1 = PID1_REQ_CONFIG;
            POUT.PID2 = PID2_TEXT_CONFIG_PACKET;   // text config packet
            POUT.LEN = (USHORT)lLen;
            break;
         case XMTPT_FULL_READ_CONFIG_PACKET:
            // CONFIG_OPTIONS Needed:
            //		CfgOptions.PC5_PRESENT
            //		CfgOptions.DppType
            strCfg = "";
            strCfg = CreateFullReadBackCmd(CfgOptions.PC5_PRESENT, CfgOptions.DppType);
            lLen = (long)strCfg.length();
            if (lLen > 0) {
                strCfg = MakeUpper(strCfg);
                CopyAsciiData(POUT.DATA, strCfg, lLen);
            }
            POUT.PID1 = PID1_REQ_CONFIG;
            POUT.PID2 = PID2_CONFIG_READBACK_PACKET;   // read config packet
            POUT.LEN = (USHORT)lLen;
            break;
        default:
            bCmdFound = FALSE;
            break;
    }
    if (bCmdFound) {
        if (! POUT_Buffer(POUT, Buffer)) {
            bCmdFound = FALSE;
        }
    }
    return bCmdFound;
}

string CSendCommand::CreateFullReadBackCmd(bool PC5_PRESENT, int DppType)
{
    string strCfg;
    bool isHVSE;
    bool isPAPS;
    bool isTECS;
    bool isVOLU;
    bool isCON1;
    bool isCON2;
    bool isINOF;
    bool isBOOT;
    bool isGATE;
    bool isPAPZ;

    isHVSE = (((DppType !=  devtypePX5) && PC5_PRESENT) || DppType == devtypePX5);
    isPAPS = (DppType !=  devtypeDP5G);
    isTECS = (((DppType == devtypeDP5) && PC5_PRESENT) || (DppType != devtypeDP5G));
    isVOLU = (DppType == devtypePX5);
    isCON1 = (DppType != devtypeDP5);
    isCON2 = (DppType != devtypeDP5);
    isINOF = (DppType != devtypeDP5G);
    isBOOT = (DppType == devtypeDP5);
    isGATE = (DppType == devtypeDP5);
    isPAPZ = (DppType == devtypePX5);

    strCfg = "";
    strCfg += "RESC=?;";
    strCfg += "CLCK=?;";
    strCfg += "TPEA=?;";
    strCfg += "GAIF=?;";
    strCfg += "GAIN=?;";
    strCfg += "RESL=?;";
    strCfg += "TFLA=?;";
    strCfg += "TPFA=?;";
    strCfg += "PURE=?;";
    strCfg += "RTDE=?;";
    strCfg += "MCAS=?;";
    strCfg += "MCAC=?;";
    strCfg += "SOFF=?;";
    strCfg += "AINP=?;";
    if (isINOF) { strCfg += "INOF=?;"; }
    strCfg += "GAIA=?;";
    strCfg += "CUSP=?;";
    strCfg += "PDMD=?;";
    strCfg += "THSL=?;";
    strCfg += "TLLD=?;";
    strCfg += "THFA=?;";
    strCfg += "DACO=?;";
    strCfg += "DACF=?;";
    strCfg += "RTDS=?;";
    strCfg += "RTDT=?;";
    strCfg += "BLRM=?;";
    strCfg += "BLRD=?;";
    strCfg += "BLRU=?;";
    if (isGATE) { strCfg += "GATE=?;"; }
    strCfg += "AUO1=?;";
    strCfg += "PRET=?;";
    strCfg += "PRER=?;";
    strCfg += "PREC=?;";
    strCfg += "PRCL=?;";
    strCfg += "PRCH=?;";
    if (isHVSE) { strCfg += "HVSE=?;"; }
    if (isTECS) { strCfg += "TECS=?;"; }
    if (isPAPZ) { strCfg += "PAPZ=?;"; }
    if (isPAPS) { strCfg += "PAPS=?;"; }
    strCfg += "SCOE=?;";
    strCfg += "SCOT=?;";
    strCfg += "SCOG=?;";
    strCfg += "MCSL=?;";
    strCfg += "MCSH=?;";
    strCfg += "MCST=?;";
    strCfg += "AUO2=?;";
    strCfg += "TPMO=?;";
    strCfg += "GPED=?;";
    strCfg += "GPIN=?;";
    strCfg += "GPME=?;";
    strCfg += "GPGA=?;";
    strCfg += "GPMC=?;";
    strCfg += "MCAE=?;";
    if (isVOLU) { strCfg += "VOLU=?;"; }
    if (isCON1) { strCfg += "CON1=?;"; }
    if (isCON2) { strCfg += "CON2=?;"; }
    if (isBOOT) { strCfg += "BOOT=?;"; }
    return strCfg;
}

//Extend Packet_Out to include entire message
//include send packet data finishing into dp5_cmd
//remove control and interface objects
//move send packet to separate function

bool CSendCommand::DP5_CMD_Data(unsigned char Buffer[], TRANSMIT_PACKET_TYPE XmtCmd, unsigned char DataOut[])
{
    bool bCmdFound;
    SHORT idxMiscData;
    Packet_Out POUT;
    long PktLen;

    bCmdFound = FALSE;
    POUT.LEN = 0;
    string strCfg;
    long idxData;
    switch (XmtCmd) {	//REQUEST_PACKETS_TO_DP5
        case XMTPT_WRITE_512_BYTE_MISC_DATA:
            POUT.PID1 = PID1_VENDOR_REQ;
            POUT.PID2 = PID2_WRITE_512_BYTE_MISC_DATA;				// write misc data
            POUT.LEN = 512;
            for (idxMiscData=0;idxMiscData<=511;idxMiscData++) {	// byte array padded w/NULLs
                POUT.DATA[idxMiscData] = DataOut[idxMiscData];
            }
            bCmdFound = TRUE;
            if (! POUT_Buffer(POUT, Buffer)) {
                bCmdFound = FALSE;
            }
            break;
         case XMTPT_SEND_TEST_PACKET:
            PktLen = (DataOut[4] * 256) + DataOut[5] + 8;		// get entire packet size
            if ((PktLen >= 8) && (PktLen <= 12)) {				// test data len 0-4 bytes
                if (TestPacketCkSumOK(DataOut)) {				// check the message for correct check sum
                    for(idxData=0;idxData<PktLen;idxData++) {	// load the data into the command buffer
                        Buffer[idxData] = DataOut[idxData];
                    }
                    bCmdFound = TRUE;
                }
            }
            break;
        default:
            bCmdFound = FALSE;
            break;
    }
    return bCmdFound;
}

bool CSendCommand::POUT_Buffer(Packet_Out POUT, unsigned char Buffer[])
{
    long idxBuffer;
    long CS;

    Buffer[0] = SYNC1_;
    Buffer[1] = SYNC2_;
    Buffer[2] = POUT.PID1;
    Buffer[3] = POUT.PID2;
    Buffer[4] = (POUT.LEN & 0xFF00) / 256;
    Buffer[5] = POUT.LEN & 0xFF;

    CS = SYNC1_ + SYNC2_ + POUT.PID1 + POUT.PID2 + ((POUT.LEN & 0xFF00) / 256) + (POUT.LEN & 0xFF);

    if (POUT.LEN > 0) {
        for (idxBuffer=0; idxBuffer<POUT.LEN;idxBuffer++) {
            Buffer[idxBuffer + 6] = POUT.DATA[idxBuffer];
            CS = CS + POUT.DATA[idxBuffer];
        }
    }
    CS = (CS ^ 0xFFFF) + 1;
    Buffer[POUT.LEN + 6] = (unsigned char)((CS & 0xFF00) / 256);
    Buffer[POUT.LEN + 7] = (unsigned char)(CS & 0xFF);
    return TRUE;
}

string CSendCommand::RemWhitespace(string strLine)
{
    unsigned int idxCh;
    string strCh;
    string strNoWSp;

    strNoWSp = "";
    if (strLine.find_first_of(Whitespace, 0) == -1) {		// string has no whitespace
        return strLine;
    } else {			// remove whitespace
        for (idxCh=0;idxCh<strLine.length();idxCh++) {
            strCh = strLine.substr(idxCh,1);
            if (strCh.find_first_of(Whitespace, 0) == -1) {		// char is not whitespace
                strNoWSp += strCh;
            }
        }
        return strNoWSp;
    }
}

string CSendCommand::GetDP5CfgStr(string strFilename)
{
    FILE *txtFile;
    char chLine[LINE_MAX];
    string strCfg;
    string strLine;
    long bytesCfg;
    long bytesLine;
    long bytesTotal;
    long lPos;
    char ch;
    stringex strfn;

    if (( txtFile = fopen(strFilename.c_str(), "r")) == NULL) {  // Can't open input file
        return "";
    }
    strCfg = "";
    while((fgets(chLine, LINE_MAX, txtFile)) != NULL) {
        strLine = strfn.Format("%s",chLine);
        bytesCfg = (long)strCfg.length();
        bytesLine = (long)strLine.length();
        bytesTotal = bytesCfg + bytesLine;
        if (bytesTotal >= DP5_MAX_CFG_SIZE) {			// if over command size quit
            break;
        } else {
            strLine = MakeUpper(strLine);			// make all uppercase
            lPos = (long)strLine.find(';');					// find the delimiter (-1=not found,0=commented line)
            if (lPos > 0) {								// if has delimiter that is not first char
                strLine = strLine.substr(0,lPos + 1);			// remove string right of delimiter
                ch = strLine.at(0);
                if ((ch >= 'A') && (ch <= 'Z')) {		// if is valid value
                    strLine = RemWhitespace(strLine);	// remove whitespace in command sequence
                    if (strLine.length() > 1) {		// check if command w/delimiter left
                        strCfg += strLine;			// add to command string
                    }
                }
            }
        }
    }
    fclose(txtFile);
    if (!(strCfg.length() > 0)) {
        return "";
    } else {
        return strCfg;
    }
}

bool CSendCommand::CopyAsciiData(unsigned char Data[], string strCfg, long lLen)
{
    long idxData;
    const char *c_str1 = strCfg.c_str();

    if (lLen > 0) {
        for(idxData=0;idxData<lLen;idxData++) {
            Data[idxData] = c_str1[idxData];
        }
        return TRUE;
    } else {
        return FALSE;
    }
    return FALSE;
}



















