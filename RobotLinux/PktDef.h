#pragma once
// Packet definition for the term project 
#include <memory>
#include <iostream>
#include <fstream>

enum CmdType { DRIVE, SLEEP, RESPONSE };
enum DIRECTION { FORWARD = 1, BACKWARD, LEFT, RIGHT };
const int HEADERSIZE = 4; 
// Calculation: 2 bytes + [1 bit + 1 bit + 1 bit + 1 bit + 4 bits]  + 1 byte 
// HEADERSIZE = 2 + 1 + 1 = 4 bytes 

#pragma pack(push, 1)
typedef struct Header {
	unsigned short int PktCount;		// 2 bytes
	unsigned short int Drive : 1;		// 1 bit
	unsigned short int Status : 1;		// 1 bit
	unsigned short int Sleep : 1;		// 1 bit
	unsigned short int Ack : 1;			// 1 bit
	unsigned short int Padding : 4;		// 4 bits
	unsigned short int Length : 8;		// 1 byte
} HEADER;

typedef struct DriveBody {
	unsigned char Direction;
	unsigned char Duration;
	unsigned char Speed;
} DRIVEBODY;

typedef struct Telemetry {
	unsigned short int LastPktCounter;
	unsigned short int CurrentGrade;
	unsigned short int HitCount;
	unsigned char LastCmd;
	unsigned char LastCmdValue;
	unsigned char LastCmdSpeed;
}TELEMETRY;
#pragma pack(pop)

class PktDef {

	struct CmdPkt {
		HEADER header;
		char* data;
		unsigned char CRC;
	}CmdPkt;

	char* RawBuffer;

public:
	PktDef();
	PktDef(char* src);
	~PktDef();
	void SetCmd(CmdType cmd);
	void SetBodyData(char* bodyData, int size);
	void SetPktCount(int count);
	CmdType GetCmd();
	bool GetAck();
	int GetLength();
	char* GetBodyData();
	int GetPktCount();
	bool CheckCRC(char* src, int size);
	void CalcCRC();
	char* GenPacket();

	// Additional methods
	void SetAck(int value); 
	unsigned char GetCRC();

	//For Testing purpose
	char* GetRawBuffer(); 
	bool IsHeaderAllZero(); 
	bool IsDriveBodyNull(); 
	int CRCCount(); 
	

	//For Debugging purpose
	void PrintHeader();
	void PrintBody();
	void PrintPkt();
};