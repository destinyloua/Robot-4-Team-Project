#pragma once
// Packet definition for the term project 
#include <memory>
#include <iostream>
#include <fstream>

enum CmdType { DRIVE, SLEEP, RESPONSE };
enum DIRECTION { FORWARD = 1, BACKWARD, LEFT, RIGHT };

class PktDef {

	struct Header {
		unsigned short int PktCount;
		unsigned short int Drive : 1;
		unsigned short int Status : 1;
		unsigned short int Sleep : 1;
		unsigned short int Ack : 1;
		unsigned short int Padding : 4;
		unsigned short int Length : 8;
	} Header;

	struct Body {
		unsigned short int Direction : 8;
		unsigned char Duration;
		unsigned char Speed;
	} Body;
	unsigned char CRC;

public:
	PktDef();
	PktDef(char* src);
	void SetCmd(CmdType cmd);
	void SetBodyData(char* cmd, int direction);
	void SetPckCount(int count);
	CmdType GetCmd();
	bool GetAck();
	int GetLength();
	char* GetBodyData();
	int GetPktCount();
	bool CheckCRC(char* src, int size);
	void CalcCRC();
	char* GenPacket();

	//For Testing purpose
	void PrintHeader();
	void PrintBody();
	void PrintPkt();
};