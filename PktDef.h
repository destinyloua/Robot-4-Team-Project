#pragma once
// Packet definition for the term project 
#include <memory>
#include <iostream>
#include <fstream>

class PktDef {

	struct Header {
		unsigned short int PktCount : 4; 
		// command flags? 
		unsigned short int Length : 4; 
	} Head;

	char* Data; 
	unsigned char CRC; 

public:
	PktDef(); 

	// need to serialize 
	
};