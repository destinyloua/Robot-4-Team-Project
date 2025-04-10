#include "MySocket.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

// configures connection and allocates memory to buffer 
MySocket::MySocket(SocketType type, std::string ip, unsigned int port, ConnectionType connType, unsigned int bufferSize)
    : mySocket(type), IPAddr(ip), Port(port), connectionType(connType), bTCPConnect(false)
{
    // buffer memory allocation 
    MaxSize = (bufferSize > 0) ? bufferSize : DEFAULT_SIZE;
    Buffer = new char[MaxSize];

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // server conditions 
    if (connectionType == TCP && type == SERVER) {
        WelcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else {
        ConnectionSocket = socket(AF_INET, connType == TCP ? SOCK_STREAM : SOCK_DGRAM, IPPROTO_UDP);
    }

    // set ip and port 
    memset(&SvrAddr, 0, sizeof(SvrAddr));
    SvrAddr.sin_family = AF_INET;
    inet_pton(AF_INET, IPAddr.c_str(), &SvrAddr.sin_addr);
    SvrAddr.sin_port = htons(Port);

    // set protocol 
    if (type == SERVER && connectionType == TCP) {
        bind(WelcomeSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr));
        listen(WelcomeSocket, SOMAXCONN);
    }
    else if (type == SERVER && connectionType == UDP) {
        bind(ConnectionSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr));
    }
}

// clean up memory 
MySocket::~MySocket() {
    if (connectionType == TCP && mySocket == SERVER) closesocket(WelcomeSocket);
    closesocket(ConnectionSocket);
    WSACleanup();
    delete[] Buffer;
}

// tcp 3 way handshake 
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

// tcp 4 way handshake 
void MySocket::DisconnectTCP() {
    if (connectionType == TCP && bTCPConnect) {
        shutdown(ConnectionSocket, SD_BOTH);
        closesocket(ConnectionSocket);
        bTCPConnect = false;
    }
}

// transmit raw data 
void MySocket::SendData(const char* data, int len) {
    if (connectionType == TCP) {
        send(ConnectionSocket, data, len, 0);
    }
    else {
        sendto(ConnectionSocket, data, len, 0, (sockaddr*)&SvrAddr, sizeof(SvrAddr));
    }
}

// receive last block of raw data internally 
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

// return configured ip 
string MySocket::GetIPAddr()
{
    return IPAddr;
}

// change ip address 
void MySocket::SetIPAddre(string ip)
{
    // error if connection already established 
    if (bTCPConnect) {
        std::cerr << "Cannot change IP address while connected." << std::endl;
        return;
    }
    IPAddr = ip;
}

// gets port number in use 
int MySocket::GetPort()
{
    return Port;
}

// change default port number 
void MySocket::SetPort(int port) {
    // error if connection already established 
    if (bTCPConnect) {
        std::cerr << "Cannot change port while connected." << std::endl;
        return;
    }
    Port = port;
}

// return the type of socket 
SocketType MySocket::GetType() {
    return mySocket; 
}

// change the socket type 
void MySocket::SetType(SocketType type) {
    if (bTCPConnect) {
        std::cerr << "Cannot change type while connected." << std::endl;
        return;
    }
    mySocket = type;
}
