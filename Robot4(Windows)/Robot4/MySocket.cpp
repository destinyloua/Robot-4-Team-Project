// CSCN72050 Final Project: MySocket implementation
// Destiny Louangsombath & Tyler Dao

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "MySocket.h"
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

MySocket::MySocket()
{
}

#pragma comment(lib, "Ws2_32.lib")

// configures connection and allocates memory to buffer 
MySocket::MySocket(SocketType type, std::string ip, unsigned int port, ConnectionType connType, unsigned int bufferSize)
    : mySocket(type), IPAddr(ip), Port(port), connectionType(connType), bTCPConnect(false)
{
    // buffer memory allocation 
    MaxSize = (bufferSize > 0) ? bufferSize : DEFAULT_SIZE;
    Buffer = new char[MaxSize];

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "ERROR: Failed to start WSA" << std::endl;
        return;
    }
    if (connectionType == UDP) {
        cout << "Setting up UDP socket..." << endl;
        ConnectionSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (ConnectionSocket == INVALID_SOCKET) {
            cerr << "ERROR: Failed to create UDP socket" << endl;
            WSACleanup();
            return;
        }
        //Setting UDP client
        if (mySocket == CLIENT) {
            cout << "Setting up CLIENT" << endl;
            SvrAddr.sin_family = AF_INET;
            SvrAddr.sin_addr.s_addr = inet_addr(ip.c_str());
            SvrAddr.sin_port = htons(Port);
            cout << "CLIENT UDP setup successfully" << endl;
        }
        //Setting UDP server
        else if (mySocket == SERVER) {
            cout << "Setting up SERVER" << endl;
            SvrAddr.sin_family = AF_INET;
            SvrAddr.sin_addr.s_addr = INADDR_ANY;
            SvrAddr.sin_port = htons(Port);
            if (bind(ConnectionSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr)) < 0) {
                cerr << "ERROR: Failed to bind" << endl;
                closesocket(ConnectionSocket);
                return;
            }
            cout << "SERVER UDP setup successfully" << endl;
        }
        return;
    }
    else if (connectionType == TCP) {
        cout << "Setting up TCP socket..." << endl;
        //Setting TCP client
        if (mySocket == CLIENT) {
            cout << "Setting up CLIENT" << endl;
            ConnectionSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (ConnectionSocket == INVALID_SOCKET) {
                cerr << "ERROR: Failed to create CLIENT TCP socket" << endl;
                WSACleanup();
                return;
            }
            //Setting TCP server address
            SvrAddr.sin_family = AF_INET;
            SvrAddr.sin_addr.s_addr = inet_addr(ip.c_str());
            SvrAddr.sin_port = htons(Port);
            cout << "CLIENT TCP set up successfully" << endl;
        }
        else if (mySocket == SERVER) {
            cout << "Setting up SERVER" << endl;
            WelcomeSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (WelcomeSocket == INVALID_SOCKET) {
                cerr << "ERROR: Failed to create SERVER TCP socket" << endl;
                WSACleanup();
                return;
            }
            //Setting TCP server address
            SvrAddr.sin_family = AF_INET;
            SvrAddr.sin_addr.s_addr = INADDR_ANY;
            SvrAddr.sin_port = htons(Port);
            cout << "SERVER TCP set up successfully" << endl;
        }
    }
}

MySocket::~MySocket() {
    if (connectionType == TCP && mySocket == SERVER) {
        closesocket(WelcomeSocket);
    }
    closesocket(ConnectionSocket);
    WSACleanup();
    delete[] Buffer;
}

void MySocket::ConnectTCP() {
    if (connectionType != TCP) {
        cerr << "ERROR: Socket is UDP" << endl;
        return;
    }
    if (mySocket == CLIENT) {
        if (connect(ConnectionSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
            cerr << "ERROR: Failed to connect to server" << endl;
            return;
        }
        else {
            cout << "CLIENT TCP connected successfully" << endl;
            bTCPConnect = true;
            return;
        }
    }
    else if (mySocket == SERVER) {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);
        if (bind(WelcomeSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
            cerr << "ERROR: Failed to bind" << endl;
            closesocket(WelcomeSocket);
            WSACleanup();
            return;
        }
        if (listen(WelcomeSocket, 1) == SOCKET_ERROR) {
            cerr << "ERROR: Failed to listen" << endl;
            closesocket(WelcomeSocket);
            WSACleanup();
            return;
        }
        ConnectionSocket = accept(WelcomeSocket, (sockaddr*)&clientAddr, &clientSize);
        if (ConnectionSocket == INVALID_SOCKET) {
            cerr << "ERROR: Failed to accept connection" << endl;
        }
        cout << "Client connected to SERVER TCP, data transmission ready..." << endl;
        bTCPConnect = true;
        return;
    }
}

// tcp 4 way handshake 
void MySocket::DisconnectTCP() {
    if (connectionType == TCP && bTCPConnect) {
        if (mySocket == CLIENT) {
            closesocket(ConnectionSocket);
            bTCPConnect = false;
            cout << "CLIENT TCP Socket Disconnected" << endl;
        }
        else {
            shutdown(ConnectionSocket, SD_BOTH);
            closesocket(WelcomeSocket);
            closesocket(ConnectionSocket);
            bTCPConnect = false;
            cout << "SERVER TCP Socket Disconnected" << endl;
        }
    }
}

// transmit raw data 
void MySocket::SendData(const char* data, int len) {
    if (connectionType == TCP) {
        int sent = send(ConnectionSocket, data, len, 0);
        if (sent == SOCKET_ERROR) {
            cerr << "ERROR: Failed to send data (TCP)";
            return;
        }
        cout << sent << " bytes of data sent successfully (TCP)" << endl;
        return;
    }
    else {
        int sent = sendto(ConnectionSocket, data, len, 0, (sockaddr*)&SvrAddr, sizeof(SvrAddr));
        if (sent == SOCKET_ERROR) {
            cerr << "ERROR: Failed to send data (UDP)";
            return;
        }
        cout << sent << " bytes of data sent successfully (UDP)" << endl;
        return;
    }
}

// receive last block of raw data internally 
int MySocket::GetData(char* outBuffer) {
    delete[] Buffer;
    Buffer = new char[MaxSize];
    if (connectionType == TCP) {
        int received = recv(ConnectionSocket, Buffer, MaxSize, 0);
        if (received == SOCKET_ERROR) {
            cerr << "ERROR: Failed to receive data (TCP)" << endl;
            return SOCKET_ERROR;
        }
        else {
            memcpy(outBuffer, Buffer, received);
            cout << received << " bytes of data received successfully (TCP)" << endl;
            return received;
        }
    }
    else {
        int SvrAddrLen = sizeof(SvrAddr);
        int received = recvfrom(ConnectionSocket, Buffer, MaxSize, 0, (sockaddr*)&SvrAddr, &SvrAddrLen);
        if (received == SOCKET_ERROR) {
            cerr << "ERROR: Failed to receive data (UDP)" << endl;
            return SOCKET_ERROR;
        }
        else {
            memcpy(outBuffer, Buffer, received);
            cout << received << " bytes of data received successfully (UDP)" << endl;
            return received;
        }
    }
}

// return configured ip 
string MySocket::GetIPAddr()
{
    return IPAddr;
}

// change ip address 
void MySocket::SetIPAddr(string ip)
{
    // error if connection already established 
    if (bTCPConnect) {
        cout << "ERROR: Cannot change IP address while connected." << endl;
        return;
    }
    IPAddr = ip;
    return;
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
        cerr << "ERROR: Cannot change port while connected." << endl;
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
        cerr << "Cannot change type while connected." << endl;
        return;
    }
    mySocket = type;
}

// return the socket type 
ConnectionType MySocket::GetConnectionType()
{
    return connectionType;
}

int MySocket::GetMaxSize()
{
    return MaxSize;
}

bool MySocket::CheckConnection()
{
    return bTCPConnect;
}
