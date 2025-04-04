#include "PktDef.h"
#include <iostream>
#include <bitset>
using namespace std; 

PktDef::PktDef() {
    this->Data = new char[3];
    DIRECTION dir = BACKWARD;
    // Define an array of values to copy into the memory block
    char data[3] = { dir, 10, 100 };

    // Use memcpy to copy data into the allocated memory
    memcpy(this->Data, data, 3);

    cout << "Direction: " << (int)this->Data[0] << std::endl;
    cout << "Duration: " << (int)this->Data[1] << std::endl;
    cout << "Speed: " << (int)this->Data[2] << std::endl;
}

PktDef::PktDef(char* src) {

} 

void PktDef::SetCmd(CmdType cmd) {

} 

void PktDef::SetBodyData(char* cmd, int direction) {

}

void PktDef::SetPckCount(int count) {

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
	return nullptr;
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
}

char* PktDef::GenPacket()
{
	return nullptr;
}

//Testing purpose
void PrintBits(char value) {
    // Print all 8 bits of the byte
    for (int i = 7; i >= 0; --i) {
        std::cout << ((value >> i) & 1);
    }
    std::cout << std::endl;
}

void PrintBody(char* driveBody) {
    // Extract and print each byte individually from driveBody (3 bytes)

    // Print Direction (byte 2)
    char byte = driveBody[0];  // Access the first byte (index 0)
    cout << "Direction: ";
    PrintBits(byte);  // Print the bits of this byte
    cout << endl;

    // Print Duration (byte 1)
    byte = driveBody[1];  // Access the second byte (index 1)
    cout << "Duration: ";
    PrintBits(byte);  // Print the bits of this byte
    cout << endl;

    // Print Speed (byte 0)
    byte = driveBody[2];  // Access the third byte (index 2)
    cout << "Speed: ";
    PrintBits(byte);  // Print the bits of this byte
    cout << endl;
}

void PktDef::PrintPkt()
{
    // Print results
    cout << "-----HEADER-----" << endl;
    cout << "Packet Count: " << std::bitset<16>(Head.PktCount) << " Dec: " << Head.PktCount << endl << endl;
    cout << "Drive: " << std::bitset<1>(Head.Drive) << " Dec: " << Head.Drive<< endl << endl;
    cout << "Status: " << std::bitset<1>(Head.Status) << " Dec: " << Head.Status << endl << endl;
    cout << "Sleep: " << std::bitset<1>(Head.Sleep) << " Dec: " << Head.Sleep << endl << endl;
    cout << "Ack: " << std::bitset<1>(Head.Ack) << " Dec: " << Head.Ack << endl << endl;
    cout << "Padding: " << std::bitset<4>(Head.Padding) << " Dec: " << Head.Padding << endl << endl;
    cout << "Length: " << std::bitset<8>(Head.Length) << " Dec: " << Head.Length << endl << endl;

    cout << "-----BODY-----" << endl;
    PrintBody(Data);

    cout << "-----TAIL-----" << endl;
    // Print CRC in decimal and binary
    cout << "CRC: " << std::bitset<8>(CRC) << static_cast<int>(CRC) << endl;
}