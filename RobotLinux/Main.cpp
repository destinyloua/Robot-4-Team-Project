#include "./PktDef.h"
#include "./MySocket.h"
#include <bitset>
#include <iostream>
using namespace std;

int main() {
    MySocket socket(CLIENT, "172.25.128.1", 25543, UDP, 1024);
    PktDef pkt;
    pkt.SetPktCount(1);
    pkt.SetCmd(DRIVE);

    char* body = new char[3];
    body[0] = static_cast<char>(FORWARD);
    body[1] = static_cast<char>(10);
    body[2] = static_cast<char>(80);
    pkt.SetBodyData(body, 3);
    delete[] body;

    socket.SendData(pkt.GenPacket(), pkt.GetLength());
    char* rx = new char[1024];
    socket.GetData(rx);

    PktDef pkt2(rx);
    pkt2.PrintPkt();

	return 1;
}