#include "PktDef.h"
#include <iostream>
#include <bitset>
using namespace std;


#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

char* sendPacketToRobot(PktDef pkt) {
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

    int received = recvfrom(sendSocket, RxBuffer, 1024, 0, reinterpret_cast<sockaddr*>(&fromAddr), &fromAddrLen);
    std::cout << received << " bytes received" << std::endl;
    if (received == SOCKET_ERROR) {
        std::cerr << "recvfrom failed. Error: " << WSAGetLastError() << std::endl;
        closesocket(sendSocket);
        WSACleanup();
        delete[] data;
        return nullptr;
    }

    if (pkt.GetCmd() == RESPONSE) {
        received = recvfrom(sendSocket, RxBuffer, 1024, 0, reinterpret_cast<sockaddr*>(&fromAddr), &fromAddrLen);
        std::cout << received << " bytes for telemetry received" << std::endl;
        if (received == SOCKET_ERROR) {
            std::cerr << "recvfrom failed. Error: " << WSAGetLastError() << std::endl;
            closesocket(sendSocket);
            WSACleanup();
            delete[] data;
            return nullptr;
        }
    }

    closesocket(sendSocket);
    WSACleanup();
    delete[] data;
    return RxBuffer;
}

char* sendPacketToRobotTCP(PktDef pkt) {
    char* data = pkt.GenPacket();
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        delete[] data;
        return nullptr;
    }

    SOCKET tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tcpSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        delete[] data;
        return nullptr;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address" << std::endl;
        closesocket(tcpSocket);
        WSACleanup();
        delete[] data;
        return nullptr;
    }

    if (connect(tcpSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        closesocket(tcpSocket);
        WSACleanup();
        delete[] data;
        return nullptr;
    }

    int pktSize = pkt.GetLength();
    int bytesSent = send(tcpSocket, data, pktSize, 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        closesocket(tcpSocket);
        WSACleanup();
        delete[] data;
        return nullptr;
    }

    std::cout << "Sent " << bytesSent << " bytes to 127.0.0.1:5000 (TCP)" << std::endl;

    char* RxBuffer = new char[1024];
    int received = recv(tcpSocket, RxBuffer, 1024, 0);
    std::cout << received << " bytes received" << std::endl;
    if (received == SOCKET_ERROR) {
        std::cerr << "recv failed. Error: " << WSAGetLastError() << std::endl;
        closesocket(tcpSocket);
        WSACleanup();
        delete[] data;
        return nullptr;
    }

    if (pkt.GetCmd() == RESPONSE) {
        received = recv(tcpSocket, RxBuffer, 1024, 0);
        std::cout << received << " bytes for telemetry received" << std::endl;
        if (received == SOCKET_ERROR) {
            std::cerr << "recv failed. Error: " << WSAGetLastError() << std::endl;
            closesocket(tcpSocket);
            WSACleanup();
            delete[] data;
            return nullptr;
        }
    }

    closesocket(tcpSocket);
    WSACleanup();
    delete[] data;
    return RxBuffer;  // Caller is responsible for deleting this
}



int main() {
    //Create PKT
    PktDef pkt;
    pkt.SetPktCount(1);
    pkt.SetCmd(RESPONSE);

    //char* body = new char[3];
    //body[0] = static_cast<char>(FORWARD);
    //body[1] = static_cast<char>(10);
    //body[2] = static_cast<char>(80);
    //pkt.SetBodyData(body, 3);
    //delete[] body;

    pkt.PrintPkt();
    //char* RxBuffer = sendPacketToRobot(pkt);

    char* pktData = pkt.GenPacket();

    cout << endl;
    char* data = sendPacketToRobotTCP(pkt);

    PktDef pkt2(data);
    delete[] pktData, data;
    pkt2.PrintPkt();

	return 1;
}