#include "PktDef.h"
#include <iostream>
#include <bitset>
#include <cstddef>

using namespace std; 

PktDef::PktDef() {
    this->RawBuffer = nullptr;
    this->CmdPkt.data = nullptr;
    this->CmdPkt.CRC = 0;
    memset(&CmdPkt.header, 0, sizeof(HEADER));

}

PktDef::PktDef(char* src) {
    this->RawBuffer = nullptr;
    memcpy(&CmdPkt.header, src, sizeof(HEADER));
    if(CmdPkt.header.Length >5){
        CmdPkt.data = new char[sizeof(DRIVEBODY)];
        memcpy(CmdPkt.data, src + sizeof(HEADER), sizeof(DRIVEBODY));
        memcpy(&CmdPkt.CRC, src + sizeof(HEADER) + sizeof(DRIVEBODY), sizeof(CmdPkt.CRC));
    }
    else {
        memcpy(&CmdPkt.CRC, src + sizeof(HEADER), sizeof(CmdPkt.CRC));
    }
}
PktDef::~PktDef()
{
}


void PktDef::SetCmd(CmdType cmd) {
    if (cmd == DRIVE) {
        this->CmdPkt.header.Drive = 1;
    }
    else if (cmd == SLEEP) {
        this->CmdPkt.header.Sleep = 1;
    }
} 

void PktDef::SetBodyData(char* bodyData, int size) {
    if(size>0){
        CmdPkt.data = new char[size];
        memcpy(CmdPkt.data, bodyData, size);
        this->CmdPkt.header.Length = 8;
        CalcCRC();
    }
}

void PktDef::SetPckCount(int count) {
    this->CmdPkt.header.PktCount = count;
}

CmdType PktDef::GetCmd()
{
	return CmdType();
}

bool PktDef::GetAck()
{
	return false;
}

int PktDef::GetLength()
{
    return CmdPkt.header.Length;
}

char* PktDef::GetBodyData()
{
    char* data = new char[sizeof(DRIVEBODY)];
    memcpy(data, CmdPkt.data, sizeof(DRIVEBODY));
    return data;
}

int PktDef::GetPktCount()
{
	return 0;
}

bool PktDef::CheckCRC(char* src, int size)
{
	return false;
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
    if (CmdPkt.header.Length > 5) {
        //Count DRIVEBODY
        for (int i = 0; i < 3; i++) {
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

char* PktDef::GenPacket()
{
    int size = sizeof(CmdPkt);
    CalcCRC();

    RawBuffer = new char[size];
    memcpy(RawBuffer, &CmdPkt.header, sizeof(HEADER));
    if (CmdPkt.header.Length > 5) {
        memcpy(RawBuffer + sizeof(HEADER), CmdPkt.data, sizeof(DRIVEBODY));
    }

    memcpy(RawBuffer +sizeof(HEADER) + sizeof(DRIVEBODY), &CmdPkt.CRC, sizeof(CmdPkt.CRC));
    return RawBuffer;
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
    unsigned char byte;
    DRIVEBODY body;
    memcpy(&body, CmdPkt.data, sizeof(DRIVEBODY));
    cout << "---------- DRIVEBODY ----------" << endl;
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
        cout << std::bitset<8>(byte)<< " ";
    }
    cout << endl;

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