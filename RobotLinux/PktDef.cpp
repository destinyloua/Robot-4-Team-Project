#include "./PktDef.h"
#include <iostream>
#include <bitset>
#include <cstddef>
#include <cstring>


using namespace std; 

// packet default constructor
PktDef::PktDef() {
    this->RawBuffer = nullptr;
    this->CmdPkt.data = nullptr;
    this->CmdPkt.CRC = 0;
    memset(&CmdPkt.header, 0, sizeof(HEADER));

}

// packet constructor with raw data buffer 
PktDef::PktDef(char* src) {
    this->RawBuffer = nullptr;
    memcpy(&CmdPkt.header, src, sizeof(HEADER));
    if(CmdPkt.header.Drive == 1){
        CmdPkt.data = new char[sizeof(DRIVEBODY)];
        memcpy(CmdPkt.data, src + sizeof(HEADER), sizeof(DRIVEBODY));
        memcpy(&CmdPkt.CRC, src + sizeof(HEADER) + sizeof(DRIVEBODY), sizeof(CmdPkt.CRC));
    }
    else if(CmdPkt.header.Length == 5){
        memcpy(&CmdPkt.CRC, src + sizeof(HEADER), sizeof(CmdPkt.CRC));
    }
    else if (CmdPkt.header.Status == 1) {
        CmdPkt.data = new char[sizeof(TELEMETRY)];
        memcpy(CmdPkt.data, src + sizeof(HEADER), sizeof(TELEMETRY));
        memcpy(&CmdPkt.CRC, src + sizeof(HEADER) + sizeof(TELEMETRY), sizeof(CmdPkt.CRC));
    }
}

// packet destructor 
PktDef::~PktDef()
{ 
}

// for setting command flag
void PktDef::SetCmd(CmdType cmd) {
    if (cmd == DRIVE) {
        this->CmdPkt.header.Drive = 1;
    }
    else if (cmd == SLEEP) {
        this->CmdPkt.header.Sleep = 1;
        this->CmdPkt.header.Length = 5;
    }
    else if (cmd == RESPONSE) {
        this->CmdPkt.header.Status = 1;
        this->CmdPkt.header.Length = 5;
    }
    CalcCRC();
} 

// for setting body data from a raw buffer 
void PktDef::SetBodyData(char* bodyData, int size) {
    if(size>0){
        CmdPkt.data = new char[size];
        memcpy(CmdPkt.data, bodyData, size);
        this->CmdPkt.header.Length = 8;
        CalcCRC();
    }
}

// for setting the packet count 
void PktDef::SetPktCount(int count) {
    this->CmdPkt.header.PktCount = count;
}

// returns the type of command as enum 
CmdType PktDef::GetCmd()
{
    if (CmdPkt.header.Drive == 1) {
        return DRIVE;
    }
    else if (CmdPkt.header.Sleep == 1) {
        return SLEEP;
    }
    else if (CmdPkt.header.Status == 1) {
        return RESPONSE;
    }
}

// returns acknowlegement - true if flag set
bool PktDef::GetAck()
{
	return CmdPkt.header.Ack == 1;
}

// returns length of data 
int PktDef::GetLength()
{
    return CmdPkt.header.Length;
}

// returns the drive body as a raw buffer 
char* PktDef::GetBodyData()
{
    if (CmdPkt.header.Drive == 1) {
        char* data = new char[sizeof(DRIVEBODY)];
        memcpy(data, CmdPkt.data, sizeof(DRIVEBODY));
        return data;
    }
    else if (CmdPkt.header.Status == 1) {
        char* data = new char[sizeof(TELEMETRY)];
        memcpy(data, CmdPkt.data, sizeof(TELEMETRY));
        return data;
    }
    return nullptr;
}

// returns the packet count
int PktDef::GetPktCount()
{
	return CmdPkt.header.PktCount;
}

// compares src crc and actual crc to see if they match 
bool PktDef::CheckCRC(char* src, int size)
{
    // check for empty source buffer 
    if (size <= 0) {
        return false; 
    }
    
    int count = 0; 

    //  recount CRC from the buffer 
    for (int i = 0; i < size - 1; ++i) {        // don't count the crc byte 
        unsigned char byte = static_cast<unsigned char>(src[i]); 
        for (int j = 0; j < 8; ++j) {
            count += (byte >> j) & 1; 
        }
    }

    // compare manually calculated crc vs. received crc
    unsigned char calculated = static_cast<unsigned char>(count); 
    unsigned char received = static_cast<unsigned char>(src[size - 1]); // holds the value of the packet's CRC 

	return calculated == received;
}

void PktDef::CalcCRC()
{
    int count = 0;
    //Count HEADER
    for (int i = 0; i < 16; i++) {
        count += (1 & (CmdPkt.header.PktCount >> i));
    }
    count += (1 & (CmdPkt.header.Drive));
    count += (1 & (CmdPkt.header.Status));
    count += (1 & (CmdPkt.header.Sleep));
    count += (1 & (CmdPkt.header.Ack));
    for (int i = 0; i < 4; i++) {
        count += (1 & (CmdPkt.header.Padding >> i));
    }
    for (int i = 0; i < 8; i++) {
        count += (1 & (CmdPkt.header.Length >> i));
    }
    if (CmdPkt.header.Drive == 1 && CmdPkt.header.Length > 5) {
        //Count DRIVEBODY
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 8; j++) {
                count += (1 & (CmdPkt.data[i] >> j));
            }
        }
    }
    else if(CmdPkt.header.Status == 1 && CmdPkt.header.Length>5)
    {
        for (int i = 0; i < sizeof(TELEMETRY); i++) {
            for (int j = 0; j < 8; j++) {
                count += (1 & (CmdPkt.data[i] >> j));
            }
        }
    }
    this->CmdPkt.CRC = static_cast<unsigned char>(count);
 /*   int count = 0;
    for (int i = 0; i < 16; i++) {
        count += 1 & (Header.PktCount >> i);
    }
    count += 1 & (Header.Drive);
    count += 1 & (Header.Status);
    count += 1 & (Header.Sleep);
    count += 1 & (Header.Ack);
    for (int i = 0; i < 4; i++) {
        count += 1 & (Header.Padding >> i);
    }

    for (int i = 0; i < 8; i++) {
        count += 1 & (Header.Length >> i);
    }

    for (int i = 0; i < 8; i++) {
        count += 1 & (Body.Direction >> i);
    }
    for (int i = 0; i < 8; i++) {
        count += 1 & (Body.Duration >> i);
    }
    for (int i = 0; i < 8; i++) {
        count += 1 & (Body.Speed >> i);
    }

    this->CRC = static_cast<unsigned char>(count);*/
}

// generates a packet (serialize) 
char* PktDef::GenPacket()
{
    int size = sizeof(CmdPkt);
    CalcCRC();

    RawBuffer = new char[size];
    memcpy(RawBuffer, &CmdPkt.header, sizeof(HEADER));
    if (CmdPkt.header.Length > 5 && CmdPkt.header.Drive == 1) {
        memcpy(RawBuffer + sizeof(HEADER), CmdPkt.data, sizeof(DRIVEBODY));
        memcpy(RawBuffer + sizeof(HEADER) + sizeof(DRIVEBODY), &CmdPkt.CRC, sizeof(CmdPkt.CRC));
    }
    else if (CmdPkt.header.Length > 5 && CmdPkt.header.Status == 1) {
        memcpy(RawBuffer + sizeof(HEADER), CmdPkt.data, sizeof(TELEMETRY));
        memcpy(RawBuffer + sizeof(HEADER) + sizeof(TELEMETRY), &CmdPkt.CRC, sizeof(CmdPkt.CRC));
    }
    else {
        memcpy(RawBuffer + sizeof(HEADER), &CmdPkt.CRC, sizeof(CmdPkt.CRC));
    }
    return RawBuffer;
}

// ADDITIONAL METHODS ADDED 
// 
// manually sets ack flag 
void PktDef::SetAck(int value)
{
    CmdPkt.header.Ack = value; 
}

// returns sthe CRC
unsigned char PktDef::GetCRC()
{
    return CmdPkt.CRC;
}

// returns the raw buffer 
char* PktDef::GetRawBuffer()
{
    return RawBuffer;
}


// METHODS FOR TESTING/DEBUGGING 
// 
// checks if all values in the header are in a safe state 
bool PktDef::IsHeaderAllZero()
{
    if (CmdPkt.header.Ack == 0 &&
        CmdPkt.header.Drive == 0 &&
        CmdPkt.header.Length == 0 &&
        CmdPkt.header.Padding == 0 &&
        CmdPkt.header.PktCount == 0 &&
        CmdPkt.header.Sleep == 0 &&
        CmdPkt.header.Status == 0) {
        return true; 
    }
    return false;
}

// checks if the drive body is null 
bool PktDef::IsDriveBodyNull()
{
    if (CmdPkt.data == nullptr) {
        return true; 
    }
    return false;
}

// alternate method for counting CRC for testing 
int PktDef::CRCCount()
{
    int count = 0; 

    // use bitset to count bits in each field this time 
    count += std::bitset<16>(CmdPkt.header.PktCount).count();
    count += std::bitset<1>(CmdPkt.header.Drive).count();
    count += std::bitset<1>(CmdPkt.header.Status).count();
    count += std::bitset<1>(CmdPkt.header.Sleep).count();
    count += std::bitset<1>(CmdPkt.header.Ack).count();
    count += std::bitset<4>(CmdPkt.header.Padding).count();
    count += std::bitset<8>(CmdPkt.header.Length).count();

    // count body bits only if header length > HEADERSIZE
    if (CmdPkt.header.Length > HEADERSIZE) {
        for (int i = 0; i < 3; i++) {
            count += std::bitset<8>(CmdPkt.data[i]).count();
        }
    }
    return count;
}


//Debugging purpose
void PktDef::PrintHeader() {
    cout << "---------- HEADER ----------" << endl;
    cout << "PktCount: " << CmdPkt.header.PktCount << endl;
    for (int i = 15; i >= 0; i--) {
        cout << (1 & CmdPkt.header.PktCount >> i);
    }
    cout << endl;

    cout << "Drive: " << CmdPkt.header.Drive << endl;
    cout << "Status: " << CmdPkt.header.Status << endl;
    cout << "Sleep: " << CmdPkt.header.Sleep << endl;
    cout << "ACK: " << CmdPkt.header.Ack << endl;

    cout << "Padding: ";
    for (int i = 3; i >= 0; i--) {
        cout << (1 & CmdPkt.header.Padding >> i);
    }
    cout << endl;

    cout << "Length: " << CmdPkt.header.Length << endl;
    for (int i = 7; i >= 0; i--) {
        cout << (1 & CmdPkt.header.Length >> i);
    }
    cout << endl;

    cout << endl;


    /*char* data;
    cout << "---------- HEADER ----------" << endl;
    cout << "PktCount: Decimal: " << this->Header.PktCount << " | Binary: ";
    for (int i = 15; i >= 0; i--) {
        cout << (1 & this->Header.PktCount >> i);
    }
    cout << endl;

    cout << "Drive: " << " | Binary: ";
    cout << (1 & this->Header.Drive) << endl;

    cout << "Status: " << " | Binary: ";
    cout << (1 & this->Header.Status) << endl;

    cout << "Sleep: " << " | Binary: ";
    cout << (1 & this->Header.Sleep) << endl;

    cout << "ACK: " << " | Binary: ";
    cout << (1 & this->Header.Ack) << endl;

    cout << "Padding: " << " | Binary: ";
    for (int i = 3; i >= 0; i--) {
        cout << (1 & this->Header.Padding >> i);
    }
    cout << endl;

    cout << "Length: " << "Decimal: " << this->Header.Length << " | Binary: ";
    for (int i = 7; i >= 0; i--) {
        cout << (1 & this->Header.Length >> i);
    }
    cout << endl;

    cout << endl;*/
}

void PktDef::PrintBody() {
    if (CmdPkt.header.Drive == 1) {
        unsigned char byte;
        DRIVEBODY body;
        memcpy(&body, CmdPkt.data, sizeof(DRIVEBODY));
        cout << "---------- DRIVE BODY ----------" << endl;
        cout << "Direction: " << static_cast<int>(body.Direction) << endl;
        byte = static_cast<unsigned char>(CmdPkt.data[0]);
        cout << std::bitset<8>(byte) << endl;

        cout << "Duration: " << static_cast<int>(body.Duration) << endl;
        byte = static_cast<unsigned char>(CmdPkt.data[1]);
        cout << std::bitset<8>(byte) << endl;

        cout << "Speed: " << static_cast<int>(body.Speed) << endl;
        byte = static_cast<unsigned char>(CmdPkt.data[2]);
        cout << std::bitset<8>(byte) << endl;

        cout << "*Data: ";
        for (int i = 0; i < 3; i++) {
            byte = static_cast<unsigned char>(CmdPkt.data[i]);
            cout << std::bitset<8>(byte) << " ";
        }
        cout << endl;
    }
    else if (CmdPkt.header.Status == 1) {
        unsigned char byte;
        TELEMETRY body;
        memcpy(&body, CmdPkt.data, sizeof(TELEMETRY));
        cout << "---------- TELEMETRY BODY ----------" << endl;
        cout << "Last Pkt Counter: " << static_cast<int>(body.LastPktCounter) << endl;
        byte = static_cast<unsigned char>(CmdPkt.data[0]);
        cout << std::bitset<16>(byte) << endl;

        cout << "Current Grade: " << static_cast<int>(body.CurrentGrade) << endl;
        byte = static_cast<unsigned char>(CmdPkt.data[2]);
        cout << std::bitset<16>(byte) << endl;

        cout << "Hit Count: " << static_cast<int>(body.HitCount) << endl;
        byte = static_cast<unsigned char>(CmdPkt.data[4]);
        cout << std::bitset<16>(byte) << endl;

        cout << "Last Cmd: " << static_cast<int>(body.LastCmd) << endl;
        byte = static_cast<unsigned char>(CmdPkt.data[6]);
        cout << std::bitset<8>(byte) << endl;

        cout << "Last Cmd Value: " << static_cast<int>(body.LastCmdValue) << endl;
        byte = static_cast<unsigned char>(CmdPkt.data[7]);
        cout << std::bitset<8>(byte) << endl;

        cout << "Last Cmd Speed: " << static_cast<int>(body.LastCmdSpeed) << endl;
        byte = static_cast<unsigned char>(CmdPkt.data[8]);
        cout << std::bitset<8>(byte) << endl;

        cout << "*Data: ";
        for (int i =0; i < sizeof(TELEMETRY); i++) {
            byte = static_cast<unsigned char>(CmdPkt.data[i]);
            cout << std::bitset<8>(byte) << " ";
        }
        cout << endl;
    }


    cout << endl;
    /*cout << "---------- BODY ----------" << endl;

    cout << "Direction: " << "Decimal: " << this->Body.Direction << " | Binary: ";
    for (int i = 7; i >= 0; i--) {
        cout << (1 & this->Body.Direction >> i);
    }
    cout << endl;

    cout << "Duration: " << "Decimal: " << static_cast<int>(this->Body.Duration) << " | Binary: ";
    for (int i = 7; i >= 0; i--) {
        cout << (1 & this->Body.Duration >> i);
    }
    cout << endl;

    cout << "Speed: " << "Decimal: " << static_cast<int>(this->Body.Speed) << " | Binary: ";
    for (int i = 7; i >= 0; i--) {
        cout << (1 & this->Body.Speed>> i);
    }
    cout << endl;

    cout << endl;*/
}

void PktDef::PrintPkt()
{
    PrintHeader();
    if (CmdPkt.header.Length > 5) {
        PrintBody();
    }
    
    
    cout << "CRC: " << static_cast<int>(CmdPkt.CRC) << endl;
    for (int i = 7; i >= 0; i--) {
        cout << (1 & CmdPkt.CRC >> i);
    }
    cout << endl;

    size_t totalSize = CmdPkt.header.Length;

    char* data = GenPacket();

    cout << "---------- RAW BUFFER ----------" << endl;
    for (size_t i = 0; i < totalSize; i++) {
        unsigned char byte = static_cast<unsigned char>(data[i]);
        cout << bitset<8>(byte) << " ";
    }
    delete[] data;
    cout << endl;
}