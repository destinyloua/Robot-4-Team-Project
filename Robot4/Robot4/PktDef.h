#pragma once
// Packet definition for the term project 
#include <memory>
#include <iostream>
#include <fstream>

class PktDef {

	struct Header {
		unsigned short int PktCount : 16;
		unsigned short int Drive : 1;
		unsigned short int Status : 1;
		unsigned short int Sleep : 1;
		unsigned short int Sleep : 1;
		unsigned short int Ack : 1;
		unsigned short int Padding : 4;
		unsigned short int Length : 8;
	} Head;

	char* Data;
	unsigned char CRC;

public:
	PktDef();
	PktDef(char* src);
	void SetCmd(); // TODO Make a CmdType class (?) then pass it to parameter
	void SetBodyData(char* cmd, int direction);
	void SetPckCount(int count);

	// need to serialize 

};