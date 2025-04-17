// CSCN72050 Final Project: MySocket implementation
// Destiny Louangsombath & Tyler Dao

#include "./MySocket.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "MySocket.h"

using namespace std;

// default constructor 
MySocket::MySocket(): bTCPConnect(false), MaxSize(DEFAULT_SIZE)
{
}

// constructor with parameters 
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

// destructor to close TCP connection and/or delete buffer 
MySocket::~MySocket() {
    if (connectionType == TCP && mySocket == SERVER) {
        close(WelcomeSocket);
    }
    close(ConnectionSocket);
    delete[] Buffer;
}

// sets up TCP connection (3 way handshake) 
void MySocket::ConnectTCP() {

    // prevent UDP socket from initiating TCP connection 
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

// terminates TCP connection (4 way handshake) 
void MySocket::DisconnectTCP() {
    if (connectionType == TCP && bTCPConnect) {
        shutdown(ConnectionSocket, SHUT_RDWR);
        close(ConnectionSocket);
        bTCPConnect = false;
        cout << "TCP connection closed successfully" << endl;
    }
    else{
        cerr << "ERROR: Socket is not connected or is UDP" << endl;
    }
}

// transmits raw data over socket 
void MySocket::SendData(const char* data, int len) {

    // TCP support
    if (connectionType == TCP) {
        int sent = send(ConnectionSocket, data, len, 0);
        if (sent < 0) {
            cerr << "ERROR: Failed to send data (TCP)";
            return;
        }
        cout << sent << " bytes of data sent successfully (TCP)" << endl;
        return;
    }
    // UDP support 
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

// retrieves last block of raw data from the socket buffer 
int MySocket::GetData(char* outBuffer) {
   
    // put received message into buffer and return bytes written 
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

// return the ip address currently configured 
string MySocket::GetIPAddr()
{
    return IPAddr;
}

// to change the ip address currently configured 
void MySocket::SetIPAddress(string ip)
{
    // prevent ip change if TCP connection already established 
    if (bTCPConnect) {
        cerr << "ERROR: Cannot change IP address while connected." << endl;
        return;
    }
    this->IPAddr = ip;
}

// return the default port number 
int MySocket::GetPort()
{
    return Port;
}

// change default port 
void MySocket::SetPort(int port) {

    // prevent port change if TCP connection already established 
    if (bTCPConnect) {
        cerr << "ERROR: Cannot change port while connected." << endl;
        return;
    }
    this->Port = port;
}

// return the type of socket (client or server) 
SocketType MySocket::GetType() {
    return mySocket; 
}

// change the socket type 
void MySocket::SetType(SocketType type) {

    // prevent socket type change is TCP connection already established 
    if (bTCPConnect) {
        cerr << "ERROR: Cannot change type while connected." << endl;
        return;
    }
    this->mySocket = type;
}

// change connection type (TCP/UDP) 
void MySocket::SetConnection(ConnectionType connType) {

    // prevent connection type change if TCP connection alreafdy established 
    if (bTCPConnect) {
        cerr << "ERROR: Cannot change connection type while connected." << endl;
        return;
    }
    this->connectionType = connType;
}

// return the status of a TCP connection (true = established) 
bool MySocket::CheckTCPConnection()
{
    return bTCPConnect;
}