#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;

const int DEFAULT_SIZE = 1024;


enum SocketType { CLIENT, SERVER};
enum ConnectionType { TCP, UDP };

class MySocket
{
	char* Buffer;
	SOCKET WelcomeSocket;
	SOCKET ConnectionSocket;
	struct sockaddr_in SvrAddr;
	SocketType mySocket;
	string IPAddr;
	int Port;
	ConnectionType connectionType;
	bool bTCPConnect;
	int MaxSize;

public:
	MySocket();
	MySocket(SocketType socketType, string ipAddress, unsigned int port, ConnectionType connectionType, unsigned int bufferSize);
	~MySocket();

	void ConnectTCP();
	void DisconnectTCP();
	void SendData(const char* data, int size);
	int GetData(char* RxBuffer);
	string GetIPAddr();
	void SetIPAddre(string ip);
	void SetPort(int port);
	int GetPort();
	SocketType GetType();
	void SetType(SocketType);
};

