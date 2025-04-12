#include "CppUnitTest.h"
#include "../Robot4/PktDef.h"
#include "../Robot4/MySocket.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Robot4TESTS
{
	TEST_CLASS(Robot4TESTS)
	{
	public:

		// CRITICAL FUNCTION TESTS 

		// test that the default constructor assigns all header values to safe state and all buffers are nullptr 
		TEST_METHOD(TEST001_DefaultConstructor_SafeState)
		{
			PktDef p;
			// check header
			Assert::IsTrue(p.IsHeaderAllZero());
			// check drive body 
			Assert::IsTrue(p.IsDriveBodyNull());
			// check raw buffer 
			Assert::IsNull(p.GetRawBuffer());
		}

		// test setting command to DRIVE 
		TEST_METHOD(TEST002_SettingCmd_DRIVE)
		{
			PktDef p;
			p.SetCmd(DRIVE);
			Assert::AreEqual((int)DRIVE, (int)p.GetCmd());

		}

		// test setting packet count accurately 
		TEST_METHOD(TEST003_SettingPktCount_50)
		{
			PktDef p;
			p.SetPktCount(50);
			Assert::AreEqual(50, (int)p.GetPktCount());

		}

		// test set body data populates the drive body 
		TEST_METHOD(TEST004_SetDriveBody_PopulatesBody)
		{
			PktDef p;
			p.SetCmd(DRIVE);
			DRIVEBODY d = { 0x01, 0x02, 0x03 };
			p.SetBodyData((char*)&d, sizeof(DRIVEBODY));

			Assert::IsFalse(p.IsDriveBodyNull());
			Assert::IsNotNull(p.GetBodyData());

		}

		// test that CRC is being calculated and set correctly 
		TEST_METHOD(TEST005_CompareCRCCalculations_SetAndMatch)
		{
			PktDef p;
			p.SetCmd(DRIVE);
			DRIVEBODY d = { 0x01, 0x02, 0x03 };
			p.SetBodyData((char*)&d, sizeof(DRIVEBODY));

			char* pRaw = p.GenPacket();
			int pSize = p.GetLength();

			bool result = p.CheckCRC(pRaw, pSize);

			Assert::IsTrue(result);

		}

		// test the ack flag is set accurately
		TEST_METHOD(TEST006_CheckACKFlag_SetAndUnset)
		{
			PktDef p;

			// set ack to 1
			p.SetAck(1);

			// check ack is 1
			Assert::IsTrue(p.GetAck());

			// set ack back to 0
			p.SetAck(0);
			Assert::IsFalse(p.GetAck());

		}

		// check value of length matches expected length 
		TEST_METHOD(TEST007_CheckLength_ActualLength)
		{
			PktDef p;

			// simulate char* data 
			char body[3] = { 0x01, 0x02, 0x03 };

			// set this body 
			p.SetBodyData(body, 3);

			// length of packet should be 3 bytes(body) + 4 byte header (HEADERSIZE) + 1 byte CRC = 8 bytes 
			int expected = 8;

			// actual length of data 
			int actual = p.GetLength();

			Assert::AreEqual(expected, actual);
		}

		// check the CRC validation identifies mismatch
		TEST_METHOD(TEST008_CRCCheck_IdentifiesMismatch)
		{
			PktDef p;
			p.SetCmd(DRIVE);
			DRIVEBODY d = { 0x01, 0x02, 0x03 };
			p.SetBodyData((char*)&d, sizeof(DRIVEBODY));

			char* pRaw = p.GenPacket();
			int pSize = p.GetLength();

			// flip bits to invoke mismatch
			pRaw[pSize - 1] ^= 0xFF;

			bool result = p.CheckCRC(pRaw, pSize);

			Assert::IsFalse(result);
		}

		// test the packet generation funtion creates a valid packet
		TEST_METHOD(TEST009_GenPacket_CreatesValidPacket)
		{
			// simulate packet data 
			PktDef p;
			p.SetCmd(DRIVE);
			p.SetPktCount(5);
			DRIVEBODY d = { 0x04, 0x05, 0x06 };
			p.SetBodyData((char*)&d, sizeof(DRIVEBODY));
			int size = p.GetLength();

			// generate a raw packet 
			char* rawPkt = p.GenPacket();

			// packet should not be null 
			Assert::IsNotNull(rawPkt);

			// turn it back into normal packet 
			PktDef p2(rawPkt);

			Assert::AreEqual((int)p.GetCmd(), (int)p2.GetCmd());
			Assert::AreEqual(p.GetPktCount(), p2.GetPktCount());

			delete[] rawPkt;
		}

		// test constructor given a raw buffer 
		TEST_METHOD(TEST010_Constructor_With_RawBuffer)
		{
			PktDef p1;
			p1.SetCmd(DRIVE);
			DRIVEBODY d = { 0x01, 0x02, 0x03 };
			p1.SetBodyData((char*)&d, sizeof(DRIVEBODY));


			char* packet = p1.GenPacket();

			PktDef p2(packet);

			Assert::AreEqual((int)p1.GetCmd(), (int)p2.GetCmd());

			delete[] packet;
		}

		TEST_METHOD(TEST011_SetSleepCommand)
		{
			PktDef p;
			p.SetCmd(SLEEP);

			Assert::AreEqual((int)SLEEP, (int)p.GetCmd());
		}

		// SOCKET TESTS 

		// tests the default constructor configures the socket and connection properly 
		TEST_METHOD(TEST012_SocketConfig_Constructor)
		{
			MySocket testSocket(SERVER, "127.0.0.1", 23500, TCP, 1000);

			Assert::AreEqual((int)SERVER, (int)testSocket.GetType());
			Assert::AreEqual((string)"127.0.0.1", testSocket.GetIPAddr());
			Assert::AreEqual(23500, testSocket.GetPort());
			Assert::AreEqual((int)TCP, (int)testSocket.GetConnectionType());
			Assert::AreEqual(1000, testSocket.GetMaxSize()); 

		}

		// test the consrtuctor uses default size when given invalid size 
		/*TEST_METHOD(TEST011_SocketConfig_InvalidSize)
		{
			MySocket testSocket(SERVER, "127.0.0.1", 23500, TCP, -1);

			Assert::IsTrue(testSocket.isServer());
			Assert::AreEqual((string)"127.0.0.1", testSocket.GetIPAddr());
			Assert::AreEqual(23500, testSocket.GetPort());
			Assert::IsTrue(testSocket.isTCP());
			Assert::AreEqual(DEFAULT_SIZE, (int)sizeof(testSocket.GetBuffer()));


		}*/
	};
}
