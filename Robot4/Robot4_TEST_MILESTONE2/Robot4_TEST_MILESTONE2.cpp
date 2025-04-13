#include "CppUnitTest.h"
#include "../Robot4/MySocket.h"
#include "../Robot4/PktDef.h"
#include <thread>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Robot4TESTMILESTONE2
{
	TEST_CLASS(Robot4TESTMILESTONE2)
	{
	public:
		TEST_METHOD(TEST01_SocketConfig_Constructor_SERVER_TCP)
		{
			MySocket testSocket(SERVER, "127.0.0.1", 23500, TCP, 1000);
			Assert::AreEqual((int)SERVER, (int)testSocket.GetType());
			Assert::AreEqual((string)"127.0.0.1", testSocket.GetIPAddr());
			Assert::AreEqual(23500, testSocket.GetPort());
			Assert::AreEqual((int)TCP, (int)testSocket.GetConnectionType());
			Assert::AreEqual(1000, testSocket.GetMaxSize());
		}
		TEST_METHOD(TEST02_SocketConfig_Constructor_CLIENT_TCP)
		{
			MySocket testSocket(CLIENT, "127.0.0.1", 23500, TCP, 1000);
			Assert::AreEqual((int)CLIENT, (int)testSocket.GetType());
			Assert::AreEqual((string)"127.0.0.1", testSocket.GetIPAddr());
			Assert::AreEqual(23500, testSocket.GetPort());
			Assert::AreEqual((int)TCP, (int)testSocket.GetConnectionType());
			Assert::AreEqual(1000, testSocket.GetMaxSize());
		}
		TEST_METHOD(TEST03_SocketConfig_Constructor_SERVER_UDP)
		{
			MySocket testSocket(SERVER, "127.0.0.1", 23500, UDP, 1000);
			Assert::AreEqual((int)SERVER, (int)testSocket.GetType());
			Assert::AreEqual((string)"127.0.0.1", testSocket.GetIPAddr());
			Assert::AreEqual(23500, testSocket.GetPort());
			Assert::AreEqual((int)UDP, (int)testSocket.GetConnectionType());
			Assert::AreEqual(1000, testSocket.GetMaxSize());
		}
		TEST_METHOD(TEST04_SocketConfig_Constructor_CLIENT_UDP)
		{
			MySocket testSocket(CLIENT, "127.0.0.1", 23500, UDP, 1000);
			Assert::AreEqual((int)CLIENT, (int)testSocket.GetType());
			Assert::AreEqual((string)"127.0.0.1", testSocket.GetIPAddr());
			Assert::AreEqual(23500, testSocket.GetPort());
			Assert::AreEqual((int)UDP, (int)testSocket.GetConnectionType());
			Assert::AreEqual(1000, testSocket.GetMaxSize());
		}
		TEST_METHOD(TEST05_SocketConfig_ConnectTCP_1)
		{
			//Multithreads because ConnectTCP() blocks the main program until a client is connected
			std::thread serverThread([] {
				MySocket server(SERVER, "127.0.0.1", 5050, TCP, 1024);
				server.ConnectTCP(); // Will block until client connects
				Assert::IsTrue(server.CheckConnection());
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(300)); // Ensure server starts first

			MySocket client(CLIENT, "127.0.0.1", 5050, TCP, 1024);
			client.ConnectTCP();
			Assert::IsTrue(client.CheckConnection());

			serverThread.join();
		}
		//Prevent ConnectionType == UDP use ConnectTcp()
		TEST_METHOD(TEST06_SocketConfig_ConnectTCP_2)
		{
			MySocket server(SERVER, "127.0.0.1", 5050, UDP, 1024);
			server.ConnectTCP();
			Assert::IsFalse(server.CheckConnection());

			MySocket client(CLIENT, "127.0.0.1", 5050, UDP, 1024);
			server.ConnectTCP();
			Assert::IsFalse(client.CheckConnection());
		}
		TEST_METHOD(TEST07_SocketConfig_DisconnectConnectTCP_1)
		{
			//Multithreads because ConnectTCP() blocks the main program until a client is connected
			std::thread serverThread([] {
				MySocket server(SERVER, "127.0.0.1", 5050, TCP, 1024);
				server.ConnectTCP(); // Will block until client connects
				Assert::IsTrue(server.CheckConnection());
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(300)); // Ensure server starts first

			MySocket client(CLIENT, "127.0.0.1", 5050, TCP, 1024);
			client.ConnectTCP();
			Assert::IsTrue(client.CheckConnection());

			client.DisconnectTCP();
			Assert::IsFalse(client.CheckConnection());

			serverThread.join();
		}
		TEST_METHOD(TEST08_SocketConfig_DisconnectConnectTCP_2)
		{
			//Multithreads because ConnectTCP() blocks the main program until a client is connected
			std::thread serverThread([] {
				MySocket server(SERVER, "127.0.0.1", 5054, TCP, 1024);
				server.ConnectTCP(); // Will block until client connects
				Assert::IsTrue(server.CheckConnection());
				server.DisconnectTCP();
				Assert::IsFalse(server.CheckConnection());
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(300)); // Ensure server starts first

			std::thread clientThread([] {
				MySocket client(CLIENT, "127.0.0.1", 5054, TCP, 1024);
				client.ConnectTCP();
				Assert::IsTrue(client.CheckConnection());
				});

			serverThread.join();
			clientThread.join();
		}

		//Test Client sends data and Server receives
		TEST_METHOD(TEST09_SocketConfig_SendData_GetData_1_TCP) {
			std::thread serverThread([] {
				MySocket server(SERVER, "127.0.0.1", 5051, TCP, 1024);
				server.ConnectTCP();
				char buffer[1024];
				int bytesReceived = server.GetData(buffer);
				buffer[bytesReceived] = '\0'; // Terminate
				Assert::AreEqual("Hello Server TCP", string(buffer).c_str());
				Assert::AreEqual((int)string(buffer).size(), bytesReceived);
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(300));

			MySocket client(CLIENT, "127.0.0.1", 5051, TCP, 1024);
			client.ConnectTCP();
			const char* msg = "Hello Server TCP";
			client.SendData(msg, strlen(msg));

			serverThread.join();
		}
		//Test Server sends data and Client receives
		TEST_METHOD(TEST10_SocketConfig_SendData_GetData_2_TCP) {
			std::thread serverThread([] {
				MySocket server(SERVER, "127.0.0.1", 5051, TCP, 1024);
				server.ConnectTCP();
				const char* msg = "Hello Client TCP";
				server.SendData(msg, strlen(msg));
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(300));

			MySocket client(CLIENT, "127.0.0.1", 5051, TCP, 1024);
			client.ConnectTCP();
			char buffer[1024];
			int bytesReceived = client.GetData(buffer);
			buffer[bytesReceived] = '\0'; // Terminate
			Assert::AreEqual("Hello Client TCP", string(buffer).c_str());
			Assert::AreEqual((int)string(buffer).size(), bytesReceived);


			serverThread.join();
		}

		TEST_METHOD(TEST11_SocketConfig_SendData_GetData_3_UDP) {
			MySocket server(SERVER, "127.0.0.1", 5051, UDP, 1024);
			MySocket client(CLIENT, "127.0.0.1", 5051, UDP, 1024);

			const char* msg1 = "Hello Server TCP";
			client.SendData(msg1, strlen(msg1));

			char received1[1024];
			int receivedBytes1 = server.GetData(received1);
			received1[receivedBytes1] = '\0';

			const char* msg2 = "Hello Client TCP";
			server.SendData(msg2, strlen(msg2));

			char received2[1024];
			int receivedBytes2 = client.GetData(received2);
			received2[receivedBytes2] = '\0';

			Assert::AreEqual(string(msg1).c_str(), received1);
			Assert::AreEqual((int)string(msg1).size(), receivedBytes1);

			Assert::AreEqual(string(msg2).c_str(), received2);
			Assert::AreEqual((int)string(msg2).size(), receivedBytes2);
		}

		TEST_METHOD(TEST12_SocketConfig_SetType_1) {
			std::thread serverThread([] {
				MySocket server(SERVER, "127.0.0.1", 5051, TCP, 1024);
				server.ConnectTCP();
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(300));

			MySocket client(CLIENT, "127.0.0.1", 5051, TCP, 1024);
			client.ConnectTCP();
			client.SetType(SERVER);
			int type = client.GetType();
			int expected = CLIENT;
			Assert::AreEqual(expected, type);
			serverThread.join();
		}
		TEST_METHOD(TEST13_SocketConfig_SetType_2) {
			std::thread serverThread([] {
				MySocket server(SERVER, "127.0.0.1", 5051, TCP, 1024);
				server.ConnectTCP();
				server.SetType(CLIENT);
				int type = server.GetType();
				int expected = SERVER;
				Assert::AreEqual(expected, type);
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(300));

			MySocket client(CLIENT, "127.0.0.1", 5051, TCP, 1024);
			client.ConnectTCP();
			serverThread.join();
		}
		TEST_METHOD(TEST14_SocketConfig_SetType_3) {
			MySocket server(SERVER, "127.0.0.1", 5051, UDP, 1024);
			MySocket client(CLIENT, "127.0.0.1", 5051, UDP, 1024);

			client.SetType(SERVER);
			int type = client.GetType();
			int expected = SERVER;
			Assert::AreEqual(expected, type);
		}

		TEST_METHOD(TEST15_SocketConfig_SetIPAddr_1) {
			std::thread serverThread([] {
				MySocket server(SERVER, "127.0.0.1", 5051, TCP, 1024);
				server.ConnectTCP();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(300));

			MySocket client(CLIENT, "127.0.0.1", 5051, TCP, 1024);
			client.ConnectTCP();
			client.SetIPAddr("127.0.0.2");
			Assert::AreEqual(string("127.0.0.1"), client.GetIPAddr());
			serverThread.join();
		}

		TEST_METHOD(TEST16_SocketConfig_SetIPAddr_2) {
			std::thread serverThread([] {
				MySocket server(SERVER, "127.0.0.1", 5051, TCP, 1024);
				server.ConnectTCP();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				server.SetIPAddr("127.0.0.2");
				Assert::AreEqual(string("127.0.0.1"), server.GetIPAddr());
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(300));

			MySocket client(CLIENT, "127.0.0.1", 5051, TCP, 1024);
			client.ConnectTCP();
			serverThread.join();
		}

		TEST_METHOD(TEST17_SocketConfig_SetIPAddr_3) {
			MySocket testSocket(CLIENT, "127.0.0.1", 5051, TCP, 1024);
			testSocket.SetIPAddr("127.0.0.2");
			Assert::AreEqual(string("127.0.0.2"), testSocket.GetIPAddr());
		}

		TEST_METHOD(TEST18_SocketConfig_SetPort_1) {
			std::thread serverThread([] {
				MySocket server(SERVER, "127.0.0.1", 5051, TCP, 1024);
				server.ConnectTCP();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(300));

			MySocket client(CLIENT, "127.0.0.1", 5051, TCP, 1024);
			client.ConnectTCP();
			client.SetPort(23500);
			Assert::AreEqual(5051, client.GetPort());
			serverThread.join();
		}

		TEST_METHOD(TEST19_SocketConfig_SetPort_2) {
			std::thread serverThread([] {
				MySocket server(SERVER, "127.0.0.1", 5051, TCP, 1024);
				server.ConnectTCP();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				server.SetPort(23500);
				Assert::AreEqual(5051, server.GetPort());
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(300));

			MySocket client(CLIENT, "127.0.0.1", 5051, TCP, 1024);
			client.ConnectTCP();
			serverThread.join();
		}

		TEST_METHOD(TEST20_SocketConfig_SetPort_3) {
			MySocket testSocket(CLIENT, "127.0.0.1", 5051, UDP, 1024);
			testSocket.SetPort(23500);
			Assert::AreEqual(23500, testSocket.GetPort());
		}
	};
}
