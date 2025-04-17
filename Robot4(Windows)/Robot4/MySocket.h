// CSCN72050 Final Project: MySocket header
// Destiny Louangsombath & Tyler Dao
#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
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
	//Done
	MySocket(SocketType socketType, string ipAddress, unsigned int port, ConnectionType connectionType, unsigned int bufferSize);
	~MySocket();

	void ConnectTCP();
	void DisconnectTCP();
	void SendData(const char* data, int size); //Done
	int GetData(char* RxBuffer);
	string GetIPAddr();
	void SetIPAddr(string ip);
	void SetPort(int port);
	int GetPort();
	SocketType GetType();
	void SetType(SocketType);

	ConnectionType GetConnectionType(); 
	int GetMaxSize(); 

	bool CheckConnection();
};

