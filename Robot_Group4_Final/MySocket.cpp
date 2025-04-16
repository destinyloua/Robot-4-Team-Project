#include "./MySocket.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "MySocket.h"

using namespace std;

MySocket::MySocket(): bTCPConnect(false), MaxSize(DEFAULT_SIZE)
{
}

MySocket::MySocket(SocketType type, std::string ip, unsigned int port, ConnectionType connType, unsigned int bufferSize)
    : mySocket(type), IPAddr(ip), Port(port), connectionType(connType), bTCPConnect(false), MaxSize(DEFAULT_SIZE)
{
    MaxSize = (bufferSize > 0) ? bufferSize : DEFAULT_SIZE;
    Buffer = new char[MaxSize];

    // UDP socket setup
    if (connectionType == UDP) {
        cout << "Setting up UDP socket..." << endl;
        ConnectionSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (ConnectionSocket < 0) {
            cerr << "ERROR: Failed to create UDP socket" << endl;
            return;
        }
        if (mySocket == CLIENT) {
            cout << "Setting up CLIENT" << endl;
            SvrAddr.sin_family = AF_INET;
            inet_pton(AF_INET, ip.c_str(), &SvrAddr.sin_addr);
            SvrAddr.sin_port = htons(Port);
            cout << "CLIENT UDP setup successfully" << endl;
        }
        else if (mySocket == SERVER) {
            cout << "Setting up SERVER" << endl;
            SvrAddr.sin_family = AF_INET;
            SvrAddr.sin_addr.s_addr = INADDR_ANY;
            SvrAddr.sin_port = htons(Port);
            if (bind(ConnectionSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr)) < 0) {
                cerr << "ERROR: Failed to bind" << endl;
                close(ConnectionSocket);
                return;
            }
            cout << "SERVER UDP setup successfully" << endl;
        }
        return;
    }

    // TCP socket setup
    else if (connectionType == TCP) {
        cout << "Setting up TCP socket..." << endl;
        if (mySocket == CLIENT) {
            cout << "Setting up CLIENT" << endl;
            ConnectionSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (ConnectionSocket < 0) {
                cerr << "ERROR: Failed to create CLIENT TCP socket" << endl;
                return;
            }
            SvrAddr.sin_family = AF_INET;
            inet_pton(AF_INET, ip.c_str(), &SvrAddr.sin_addr);
            SvrAddr.sin_port = htons(Port);
            cout << "CLIENT TCP set up successfully" << endl;
        }
        else if (mySocket == SERVER) {
            cout << "Setting up SERVER" << endl;
            WelcomeSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (WelcomeSocket < 0) {
                cerr << "ERROR: Failed to create SERVER TCP socket" << endl;
                return;
            }
            SvrAddr.sin_family = AF_INET;
            SvrAddr.sin_addr.s_addr = INADDR_ANY;
            SvrAddr.sin_port = htons(Port);
            cout << "SERVER TCP set up successfully" << endl;
        }
    }
}

MySocket::~MySocket() {
    if (connectionType == TCP && mySocket == SERVER) {
        close(WelcomeSocket);
    }
    close(ConnectionSocket);
    delete[] Buffer;
}

void MySocket::ConnectTCP() {
    if (connectionType != TCP) {
        cerr << "ERROR: Socket is UDP" << endl;
        return;
    }
    if (mySocket == CLIENT) {
        if (connect(ConnectionSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr)) < 0) {
            cerr << "ERROR: Failed to connect to server" << endl;
            bTCPConnect = false;
            cout <<"IP: "<< IPAddr<<endl;
            cout <<"Port: "<< Port<<endl;
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
        socklen_t clientSize = sizeof(clientAddr);
        ConnectionSocket = accept(WelcomeSocket, (sockaddr*)&clientAddr, &clientSize);
        if (ConnectionSocket < 0) {
            cerr << "ERROR: Failed to accept connection" << endl;
            cout <<"IP: "<< IPAddr<<endl;
            cout <<"Port: "<< Port<<endl;
            bTCPConnect = false;
            return;
        }
        cout << "Client connected to SERVER TCP, data transmission ready..." << endl;
        bTCPConnect = true;
        return;
    }
}

void MySocket::DisconnectTCP() {
    if (connectionType == TCP && bTCPConnect) {
        shutdown(ConnectionSocket, SHUT_RDWR);
        close(ConnectionSocket);
        bTCPConnect = false;
    }
}

void MySocket::SendData(const char* data, int len) {
    if (connectionType == TCP) {
        int sent = send(ConnectionSocket, data, len, 0);
        if (sent < 0) {
            cerr << "ERROR: Failed to send data (TCP)";
            return;
        }
        cout << sent << " bytes of data sent successfully (TCP)" << endl;
        return;
    }
    else {
        int sent = sendto(ConnectionSocket, data, len, 0, (sockaddr*)&SvrAddr, sizeof(SvrAddr));
        if (sent < 0) {
            cerr << "ERROR: Failed to send data (UDP)";
            return;
        }
        cout << sent << " bytes of data sent successfully (UDP)" << endl;
        return;
    }
}

int MySocket::GetData(char* outBuffer) {
    Buffer = new char[MaxSize];
    if (connectionType == TCP) {
        int received = recv(ConnectionSocket, Buffer, MaxSize, 0);
        if (received < 0) {
            cerr << "ERROR: Failed to receive data (TCP)" << endl;
            return -1;
        }
        if (received <= MaxSize) {
            memcpy(outBuffer, Buffer, received);
            cout << received << " bytes of data received successfully (TCP)" << endl;
            return received;
        }
        else {
            cerr << "ERROR: Data overflow, MaxSize is " << MaxSize << " bytes but " << received << " bytes of data were received (TCP)" << endl;
            return -1;
        }
    }
    else {
        socklen_t SvrAddrLen = sizeof(SvrAddr);
        int received = recvfrom(ConnectionSocket, Buffer, MaxSize, 0, (sockaddr*)&SvrAddr, &SvrAddrLen);
        if (received < 0) {
            cerr << "ERROR: Failed to receive data (UDP)" << endl;
            return -1;
        }
        if (received <= MaxSize) {
            memcpy(outBuffer, Buffer, received);
            delete[] Buffer; // Free the buffer after use
            cout << received << " bytes of data received successfully (UDP)" << endl;
            return received;
        }
        else {
            cerr << "ERROR: Data overflow, MaxSize is " << MaxSize << " bytes but " << received << " bytes of data were received (UDP)" << endl;
            return -1;
        }
    }
}

string MySocket::GetIPAddr()
{
    return IPAddr;
}

void MySocket::SetIPAddress(string ip)
{
    if (bTCPConnect) {
        cerr << "ERROR: Cannot change IP address while connected." << endl;
        return;
    }
    this->IPAddr = ip;
}

int MySocket::GetPort()
{
    return Port;
}

void MySocket::SetPort(int port) {
    if (bTCPConnect) {
        cerr << "ERROR: Cannot change port while connected." << endl;
        return;
    }
    this->Port = port;
}

SocketType MySocket::GetType() {
    return mySocket; 
}

void MySocket::SetType(SocketType type) {
    if (bTCPConnect) {
        cerr << "ERROR: Cannot change type while connected." << endl;
        return;
    }
    this->mySocket = type;
}

void MySocket::SetConnection(ConnectionType connType) {
    if (bTCPConnect) {
        cerr << "ERROR: Cannot change connection type while connected." << endl;
        return;
    }
    this->connectionType = connType;
}

bool MySocket::CheckTCPConnection()
{
    return bTCPConnect;
}