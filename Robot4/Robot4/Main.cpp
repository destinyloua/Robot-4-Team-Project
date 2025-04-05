#include "PktDef.h"
#include <iostream>
#include <bitset>
using namespace std;


#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

// Function to send a PktDef packet over UDP to the robot.
char* sendPacketToRobot(PktDef pkt) {
    // Generate the raw packet data from the PktDef object.
    // Calculate the packet size: assume it's Header + Body + CRC.
    char* data = new char[sizeof(pkt)];
    data = pkt.GenPacket();
    // Initialize Winsock.
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        delete[] data;
        return nullptr;
    }

    // Create a UDP socket.
    SOCKET sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        delete[] data;
        return nullptr;
    }

    // Set up the destination address (robot's IP and port).
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

    // Send the packet.
    int bytesSent = sendto(sendSocket, data, sizeof(pkt), 0,
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
    if (received == SOCKET_ERROR) {
        std::cerr << "recvfrom failed. Error: " << WSAGetLastError() << std::endl;
    }
    // Clean up.
    closesocket(sendSocket);
    WSACleanup();
    delete[] data;  // Option 1: Caller is responsible for freeing the memory allocated in GenPacket().
    return RxBuffer;
}


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
        //pkt.PrintPkt();
        data = sendPacketToRobot(pkt);

        PktDef rec(data);
        rec.PrintPkt();
    }
	return 1;
}