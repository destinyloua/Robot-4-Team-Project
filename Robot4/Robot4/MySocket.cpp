#include "MySocket.h"
#include <iostream>

MySocket::MySocket(SocketType type, std::string ip, unsigned int port, ConnectionType connType, unsigned int bufferSize)
    : mySocket(type), IPAddr(ip), Port(port), connectionType(connType), bTCPConnect(false)
{
    MaxSize = (bufferSize > 0) ? bufferSize : DEFAULT_SIZE;
    Buffer = new char[MaxSize];

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (connectionType == TCP && type == SERVER) {
        WelcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else {
        ConnectionSocket = socket(AF_INET, connType == TCP ? SOCK_STREAM : SOCK_DGRAM, IPPROTO_UDP);
    }

    memset(&SvrAddr, 0, sizeof(SvrAddr));
    SvrAddr.sin_family = AF_INET;
    inet_pton(AF_INET, IPAddr.c_str(), &SvrAddr.sin_addr);
    SvrAddr.sin_port = htons(Port);

    if (type == SERVER && connectionType == TCP) {
        bind(WelcomeSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr));
        listen(WelcomeSocket, SOMAXCONN);
    }
    else if (type == SERVER && connectionType == UDP) {
        bind(ConnectionSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr));
    }
}

MySocket::~MySocket() {
    if (connectionType == TCP && mySocket == SERVER) closesocket(WelcomeSocket);
    closesocket(ConnectionSocket);
    WSACleanup();
    delete[] Buffer;
}

void MySocket::ConnectTCP() {
    if (connectionType != TCP) {
        std::cerr << "ConnectTCP called on a UDP socket." << std::endl;
        return;
    }
    if (mySocket == CLIENT) {
        if (connect(ConnectionSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
            std::cerr << "TCP Connect failed: " << WSAGetLastError() << std::endl;
        }
        else {
            bTCPConnect = true;
        }
    }
    else if (mySocket == SERVER) {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);
        ConnectionSocket = accept(WelcomeSocket, (sockaddr*)&clientAddr, &clientSize);
        if (ConnectionSocket != INVALID_SOCKET) bTCPConnect = true;
    }
}

void MySocket::DisconnectTCP() {
    if (connectionType == TCP && bTCPConnect) {
        shutdown(ConnectionSocket, SD_BOTH);
        closesocket(ConnectionSocket);
        bTCPConnect = false;
    }
}

void MySocket::SendData(const char* data, int len) {
    if (connectionType == TCP) {
        send(ConnectionSocket, data, len, 0);
    }
    else {
        sendto(ConnectionSocket, data, len, 0, (sockaddr*)&SvrAddr, sizeof(SvrAddr));
    }
}

int MySocket::GetData(char* outBuffer) {
    int bytesReceived = 0;
    if (connectionType == TCP) {
        bytesReceived = recv(ConnectionSocket, Buffer, MaxSize, 0);
    }
    else {
        sockaddr_in from;
        int fromLen = sizeof(from);
        bytesReceived = recvfrom(ConnectionSocket, Buffer, MaxSize, 0, (sockaddr*)&from, &fromLen);
    }
    if (bytesReceived > 0) {
        memcpy(outBuffer, Buffer, bytesReceived);
    }
    return bytesReceived;
}

string MySocket::GetIPAddr()
{
    return IPAddr;
}

void MySocket::SetIPAddre(string ip)
{
    if (bTCPConnect) {
        std::cerr << "Cannot change IP address while connected." << std::endl;
        return;
    }
    IPAddr = ip;
}

int MySocket::GetPort()
{
    return Port;
}

void MySocket::SetPort(int port) {
    if (bTCPConnect) {
        std::cerr << "Cannot change port while connected." << std::endl;
        return;
    }
    Port = port;
}

SocketType MySocket::GetType() {
    return mySocket; 
}

void MySocket::SetType(SocketType type) {
    if (bTCPConnect) {
        std::cerr << "Cannot change type while connected." << std::endl;
        return;
    }
    mySocket = type;
}
