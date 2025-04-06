#include "PktDef.h"
#include <iostream>
#include <bitset>
using namespace std;


int main() {
    int pktCounter = 0;
    while (true) {
        char* data = new char[3];
        int direction;
        int option;
        cout << "1. Forward" << endl << "2. Backward" << endl << "3. Right" << endl << "4. Left" << endl;
        cout << "Select direction: ";
        cin >> option;
        switch (option)
        {
        case 1:
            direction = FORWARD;
            break;
        case 2:
            direction = BACKWARD;
            break;
        case 3:
            direction = RIGHT;
            break;
        case 4:
            direction = LEFT;
            break;
        default:
            direction = 0;
            break;
        }
        cout << "Select duration (second): ";
        int duration;
        cin >> duration;
        
        cout << "Select speed (80-100): ";
        int speed;
        cin >> speed;

        data[0] = direction;
        data[1] = duration;
        data[2] = speed;

        PktDef pkt;
        pktCounter++;
        pkt.SetPckCount(pktCounter);
        pkt.SetCmd(DRIVE);
        pkt.SetBodyData(data, 3);
        
        delete[] data;

        char* pktData = new char;
        pktData = pkt.GenPacket();

        PktDef rec(pktData);
        rec.PrintPkt();
    }
	return 1;
}