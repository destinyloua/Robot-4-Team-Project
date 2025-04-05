#include "PktDef.h"
#include <iostream>
#include <bitset>
using namespace std;

int main() {
	PktDef pkt;
	pkt.SetCmd(DRIVE);
	char* cmd = new char[2];
	cmd[0] = 10;
	cmd[1] = 100;
	pkt.SetBodyData(cmd, FORWARD);
	delete[] cmd;
	
	pkt.SetPckCount(2);

	//cmd = pkt.GetBodyData();
	//cout << "CMD: ";
	//cout << cmd << endl;
	char* data = pkt.GenPacket();

	PktDef pkt2(data);
	delete[] data;
	pkt2.PrintPkt();
	return 1;
}