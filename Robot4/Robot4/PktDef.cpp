#include "PktDef.h"
#include <iostream>
#include <bitset>
#include <cstddef>

using namespace std; 

PktDef::PktDef() {
    memset(&Header, 0, sizeof(Header));
    memset(&Body, 0, sizeof(Body));
    CRC = 0;
}

PktDef::PktDef(char* src) {
    memcpy(&this->Header, src, sizeof(this->Header));

    // Extract Body (starts after Header)
    memcpy(&this->Body, src + sizeof(this->Header), sizeof(this->Body));

    memcpy(&this->CRC, src + sizeof(this->Header) + sizeof(this->Body), sizeof(unsigned char));

    CalcCRC();
} 

void PktDef::SetCmd(CmdType cmd) {
    if (cmd == DRIVE) {
        this->Header.Drive = 1;
        this->Header.Length = 7;
    }
    else if (cmd == SLEEP) {
        this->Header.Sleep = 1;
        this->Header.Length = 4;

    }
} 

void PktDef::SetBodyData(char* data, int direction) {
    this->Body.Direction = direction;
    memcpy(&this->Body.Duration, data, 2);
}

void PktDef::SetPckCount(int count) {
    this->Header.PktCount = count;
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
	return 0;
}

char* PktDef::GetBodyData()
{
    char* body = new char[sizeof(Body)];
    memcpy(body, &this->Body, sizeof(Body));
	return body;
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
    count += 1 & (Header.Length);

    for (int i = 0; i < 8; i++) {
        count += 1 & (Body.Direction >> i);
    }
    for (int i = 0; i < 8; i++) {
        count += 1 & (Body.Duration >> i);
    }
    for (int i = 0; i < 8; i++) {
        count += 1 & (Body.Speed >> i);
    }

    this->CRC = static_cast<unsigned char>(count);
}

char* PktDef::GenPacket()
{
    size_t size = sizeof(this->Header) + sizeof(this->Body) + sizeof(this->CRC);
    char* Data = new char[size];
    memcpy(Data, &this->Header, sizeof(this->Header));
    memcpy(Data + sizeof(Header), &this->Body, sizeof(Body));
    memcpy(Data + sizeof(Header) + sizeof(Body), &this->CRC, sizeof(CRC));
	return Data;
}

//Testing purpose
void PktDef::PrintHeader() {
    char* data;
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

    cout << endl;
}

void PktDef::PrintBody() {
    cout << "---------- BODY ----------" << endl;

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

    cout << endl;
}

void PktDef::PrintPkt()
{
    PrintHeader();
    PrintBody();
    
    cout << "CRC: " << "Decimal: " << static_cast<int>(this->CRC) << " | Binary: ";
    for (int i = 7; i >= 0; i--) {
        cout << (1 & this->CRC >> i);
    }
    cout << endl;
}