// CSCN72050 Final Project: Command and Control GUI 
// Destiny Louangsombath & Tyler Dao  

#define CROW_MAIN
#include "./PktDef.h"
#include "./MySocket.h"

#include "crow_all.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctype.h>
using namespace std;
using namespace crow; 

// reusable function for loading files 
void get_file(const string& fileName, const string& content_type, response& res) {
	ifstream in(fileName, ifstream::in);
	if (in) {
		ostringstream contents; 
		contents << in.rdbuf(); 
		in.close(); 
		res.set_header("Content-Type", content_type);
		res.write(contents.str());
	}
	else {  
		res.code = 404; // not found
		res.set_header("Content-Type", "text/plain");
		res.write("File not found"); 
	} 
	res.end(); 
}

string ResponseToString(PktDef reqpkt, PktDef respkt) {
	if(reqpkt.GetCmd() == DRIVE) {
		if(respkt.GetAck() == 1){
			char * body = reqpkt.GetBodyData();
			int direction = static_cast<int>(body[0]);
			int duration = static_cast<int>(body[1]);
			int speed = static_cast<int>(body[2]);
			if(direction == 1){
				return "{\"direction\": \"forward\", \"duration\": \"" + to_string(duration) + "\", \"speed\":\"" + to_string(speed) + "\"}";
			}
			else if(direction == 2){
				return "{\"direction\": \"backward\", \"duration\": \"" + to_string(duration) + "\", \"speed\":\"" + to_string(speed) + "\"}";
			}
			else if(direction == 3){
				return "{\"direction\": \"left\", \"duration\": \"" + to_string(duration) + "\", \"speed\":\"" + to_string(speed) + "\"}";
			}
			else if(direction == 4){
				return "{\"direction\": \"right\", \"duration\": \"" + to_string(duration) + "\", \"speed\":\"" + to_string(speed) + "\"}";
			}
		}
		else{
			return "Drive command failed";
		}
	}

	else if(reqpkt.GetCmd() == SLEEP) {
		if(respkt.GetAck() == 1){
			return "Robot is going to sleep\n";
		}
		else{
			return "Sleep command failed";
		}
	}

	else if(reqpkt.GetCmd() == RESPONSE) {
		if(respkt.GetLength() == 5 && respkt.GetAck() == 1){
			return "Telemetry command recived, sending back data...";
		}
		else if(respkt.GetLength() > 5){
			TELEMETRY tele;
			memcpy(&tele, respkt.GetBodyData(), sizeof(TELEMETRY));
			int lastPktCount = static_cast<int>(tele.LastPktCounter);
			int currentGrade = static_cast<int>(tele.CurrentGrade);
			int hitCount = static_cast<int>(tele.HitCount);
			int lastCmdValue = static_cast<int>(tele.LastCmdValue);
			int lastCmdSpeed = static_cast<int>(tele.LastCmdSpeed);
			string lastCmd;
			if(static_cast<int>(tele.LastCmd)== 1){
				lastCmd = "forward";
			}
			else if(static_cast<int>(tele.LastCmd)== 2){
				lastCmd = "backward";
			}
			else if(static_cast<int>(tele.LastCmd)== 3){
				lastCmd = "left";
			}
			else if(static_cast<int>(tele.LastCmd)== 4){
				lastCmd = "right";
			}
			else{
				lastCmd = "unknown";
			}

			return "{\"lastPktCount\": \"" + to_string(lastPktCount) + "\", \"currentGrade\": \"" + to_string(currentGrade) + "\", \"hitCount\": \"" + to_string(hitCount) + "\", \"lastCmd\": \"" + lastCmd + "\", \"lastCmdValue\": \"" + to_string(lastCmdValue) + "\", \"lastCmdSpeed\": \"" + to_string(lastCmdSpeed) + "\"}";
		}
		else{
			return "Telemetry command failed";
		}
	}
}


int main()
{
	int pktCount = 0;
	crow::SimpleApp app;
	MySocket socket;
	// homepage (default route)
	CROW_ROUTE(app, "/")
		([](const request &req, response &res) { 
		get_file("../public/index.html", "text/html", res);
		
		});

	// for getting stylesheet 
	CROW_ROUTE(app, "/styles/<string>").methods(HTTPMethod::Get)
		([](const request& req, response& res, string fileName) { 
		get_file("../public/styles/" + fileName, "text/css", res);

		});

	// for getting javascript file
	CROW_ROUTE(app, "/scripts/<string>").methods(HTTPMethod::Get)
		([](const request& req, response& res, string fileName) { 
		get_file("../public/scripts/" + fileName, "application/javascript", res);

		});

	// for connecting  
	CROW_ROUTE(app, "/connect/<string>/<int>").methods(HTTPMethod::Post)
		([&socket](const request& req, response& res, string ipAddress, int port) {
			// TODO: logic to connect goes here
			socket.SetIPAddress(ipAddress);
			socket.SetPort(port);
			socket.SetType(CLIENT);
			socket.SetConnection(UDP);
			socket.ConnectUDP();

			res.code = 200;
			res.write("Connected to robot at " + ipAddress + ":" + to_string(port) + " by UDP connection (Connectionless)\n");
			res.end();
		});

	// for connecting  
	CROW_ROUTE(app, "/connect").methods(HTTPMethod::Get)
	([](const request& req, response& res) {
		// TODO: logic to connect goes here
		get_file("../public/connect.html", "text/html", res);
	});

	// for sending robot /tele command
	CROW_ROUTE(app, "/telecommand").methods(HTTPMethod::Put)
		([&pktCount, &socket](const request& req, response& res) {
			auto cmd = req.url_params.get("cmd");
			if(cmd && string(cmd) == "drive"){
				int direction =stoi(req.url_params.get("direction"));
				int duration = stoi(req.url_params.get("duration"));
				int speed = stoi(req.url_params.get("speed"));

				char* body = new char[3];
				body[0] = static_cast<char>(direction);
				body[1] = static_cast<char>(duration); // duration
				body[2] = static_cast<char>(speed); // speed
				PktDef reqpkt;
				pktCount++;
				reqpkt.SetPktCount(pktCount);
				reqpkt.SetCmd(DRIVE);
				reqpkt.SetBodyData(body, 3);
				socket.SendData(reqpkt.GenPacket(), reqpkt.GetLength());

				char* rx = new char[DEFAULT_SIZE];
				int received = socket.GetData(rx);
				if (received <= 0) {
					delete[] rx;
					res.code = 500;
					res.write("Failed to receive data from robot");
					res.end();
					return;
				}
				PktDef respkt(rx);

				delete[] rx;
				string response = ResponseToString(reqpkt, respkt);
				res.code = 200;
				res.write(response);
				res.end();
			}
			else if(cmd && string(cmd) == "sleep"){
				PktDef reqpkt;
				pktCount++;
				reqpkt.SetPktCount(pktCount);
				reqpkt.SetCmd(SLEEP);
				socket.SendData(reqpkt.GenPacket(), reqpkt.GetLength());
				char * rx = new char[DEFAULT_SIZE];
				int received = socket.GetData(rx);
				if (received <= 0) {
					delete[] rx;
					res.code = 500;
					res.write("Failed to receive data from robot");
					res.end();
					return;
				}
				PktDef respkt(rx);
				delete[] rx;
				string response = ResponseToString(reqpkt, respkt);
				res.code = 200;
				res.write(response);
				res.end();
			}
			else{
				res.code = 400; // bad request
				res.write("Invalid command\n");
				res.end();
			}
		});

	// for getting telemetry response
	CROW_ROUTE(app, "/telemetry/request/").methods(HTTPMethod::Get)
		([&pktCount, &socket](const request& req, response& res) {
			PktDef reqpkt;
			reqpkt.SetCmd(RESPONSE);
			pktCount++;
			reqpkt.SetPktCount(pktCount);
			socket.SendData(reqpkt.GenPacket(), reqpkt.GetLength());

			char* rx = new char[DEFAULT_SIZE];
			int received = socket.GetData(rx);
			if (received <= 0) {
				delete[] rx;
				res.code = 500;
				res.write("Failed to receive data from robot");
				res.end();
				return;
			}
			PktDef respkt(rx);
			delete[] rx;
			if(respkt.GetAck() == 0){
				res.code = 500;
				res.write("Failed to receive telemetry data from robot");
				res.end();
				return;
			}
			else{
				rx = new char[DEFAULT_SIZE];
				received = socket.GetData(rx);
				if (received <= 0) {
					delete[] rx;
					res.code = 500;
					res.write("Failed to receive data from robot");
					res.end();
					return;
				}
				PktDef telePkt(rx);
				delete[] rx;
				string response = ResponseToString(reqpkt, telePkt);
				res.code = 200;
				res.write(response);
				res.end();
			}
		});
	app.port(25543).multithreaded().run();
	return 1;
}