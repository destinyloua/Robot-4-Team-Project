#include "PktDef.h"
#include <iostream>
#include <bitset>
using namespace std;


#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

PktDef sendPacketToRobot(PktDef pkt) {
    char* data = pkt.GenPacket();
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        delete[] data;
        return nullptr;
    }

    SOCKET sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        delete[] data;
        return nullptr;
    }

    sockaddr_in destAddr;
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(5000);
    if (inet_pton(AF_INET, "127.0.0.1", &destAddr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address" << std::endl;
        closesocket(sendSocket);
        WSACleanup();
        delete[] data;
        return nullptr;
    }

    int pktSize = pkt.GetLength();
    int bytesSent = sendto(sendSocket, data, pktSize, 0,
        reinterpret_cast<sockaddr*>(&destAddr), sizeof(destAddr));
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        closesocket(sendSocket);
        WSACleanup();
        delete[] data;
        return nullptr;
    }

    std::cout << "Sent " << bytesSent << " bytes to " << "127.0.0.1" << ":" << 5000 << std::endl;

    sockaddr_in fromAddr;
    int fromAddrLen = sizeof(fromAddr);
    char* RxBuffer = new char[1024];
    int len = sizeof(struct sockaddr_in);
    int received = recvfrom(sendSocket, RxBuffer, 1024, 0, reinterpret_cast<sockaddr*>(&fromAddr), &fromAddrLen);
    std::cout << received << " bytes received" << std::endl;
    if (received == SOCKET_ERROR) {
        std::cerr << "recvfrom failed. Error: " << WSAGetLastError() << std::endl;
        closesocket(sendSocket);
        WSACleanup();
        delete[] data;
        return NULL;
    }

    PktDef res(RxBuffer);
    //TODO Figure it out the telemetry response
    //if (res.GetCmd() == RESPONSE) {
    //    delete[] RxBuffer;
    //    RxBuffer = new char[1024];
    //    len = sizeof(struct sockaddr_in);
    //    int received = recvfrom(sendSocket, RxBuffer, 1024, 0, reinterpret_cast<sockaddr*>(&fromAddr), &fromAddrLen);
    //    if (received == SOCKET_ERROR) {
    //        std::cerr << "recvfrom failed. Error: " << WSAGetLastError() << std::endl;
    //        closesocket(sendSocket);
    //        WSACleanup();
    //        delete[] data;
    //        delete[] RxBuffer;
    //        return NULL;
    //    }
    //    std::cout << received << " bytes received" << std::endl;
    //    PktDef telemetryPkt(RxBuffer);
    //    closesocket(sendSocket);
    //    WSACleanup();
    //    delete[] data;
    //    return telemetryPkt;
    //}

    closesocket(sendSocket);
    WSACleanup();
    delete[] data;
    return res;
}


int main() {
    //Create PKT
    PktDef pkt;
    pkt.SetPckCount(1);
    //pkt.SetCmd(DRIVE);

    //char* data = new char[3];
    //data[0] = static_cast<char>(FORWARD);
    //data[1] = static_cast<char>(10);
    //data[2] = static_cast<char>(80);
    //pkt.SetBodyData(data, 3);
    //delete[] data;

    pkt.SetCmd(RESPONSE);
    pkt.PrintPkt();
    //char* RxBuffer = sendPacketToRobot(pkt);

    char* pktData = pkt.GenPacket();

    cout << endl;

    PktDef pkt2 = sendPacketToRobot(pkt);
    delete[] pktData;
    pkt2.PrintPkt();
	return 1;
}