#include "CppUnitTest.h"
#include "../Robot4/PktDef.h"

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

			// simulate char* data 
			char body[3] = { 0x01, 0x02, 0x03 };

			// set this body 
			p.SetBodyData(body, 3);
			
			// retrieve the body as a raw buffer 
			char* data = p.GetBodyData();  

			Assert::AreEqual((char)0x01, data[0]); 
			Assert::AreEqual((char)0x02, data[1]);
			Assert::AreEqual((char)0x03, data[2]);

			delete[] data; 
		}

		// test that CRC is being calculated and set correctly 
		TEST_METHOD(TEST005_CompareCRCCalculations_SetAndMatch)
		{
			PktDef p;

			// simulate char* data 
			char body[3] = { 0x01, 0x02, 0x03 };

			// set this body 
			p.SetBodyData(body, 3);

			// call the CRC calculation 
			p.CalcCRC(); 

			// compare with alternate CRC count method 
			int expected = p.CRCCount(); 
			unsigned char actual = p.GetCRC(); 

			Assert::AreEqual((unsigned char)expected, actual); 

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

		// check the CRC validation works as expected 
		TEST_METHOD(TEST008_CRCCheck_MatchesCorrectly)
		{
			PktDef p; 

			// simulate char* data 
			char body[3] = { 0x01, 0x02, 0x03 };

			// set this body 
			p.SetBodyData(body, 3);

			// call the CRC calculation 
			p.CalcCRC();

			// generate a packet 
			char* testPkt = p.GenPacket(); 
			int length = p.GetLength(); 

			Assert::IsTrue(p.CheckCRC(testPkt, length)); 

			delete[] testPkt; 
		}

		// test the packet generation funtion creates a valid packet
		TEST_METHOD(TEST009_GenPacket_CreatesValidPacket)
		{
			// simulate packet data 
			PktDef p; 
			p.SetCmd(DRIVE); 
			p.SetPktCount(5); 
			char body[3] = { 0x04, 0x05, 0x06 }; 
			p.SetBodyData(body, 3);

			// generate a packet 
			char* rawPkt = p.GenPacket(); 

			// packet should not be null 
			Assert::IsNotNull(rawPkt); 

			// check the drive bit is set 
			unsigned char headerByte = rawPkt[0];			// header byte
			bool isDriveSet = (headerByte & 0x01) != 0;		// drive bit is the first bit after pktcount 
			Assert::IsTrue(isDriveSet); 

			// packet count should be 5
			Assert::AreEqual((unsigned char)5, (unsigned char)rawPkt[0]);

			// size of packet should be 8 (header(4) + drive body(3) + CRC(1)) 
			Assert::AreEqual((HEADERSIZE + 3 + 1), (int)sizeof(rawPkt)); 

			delete[] rawPkt; 
		}
	};
}
