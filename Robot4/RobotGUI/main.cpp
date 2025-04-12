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
				return "Robot is moving forward for " + to_string(duration) + " seconds at speed " + to_string(speed) + "\n";
			}
			else if(direction == 2){
				return "Robot is moving backward for " + to_string(duration) + " seconds at speed " + to_string(speed) + "\n";
			}
			else if(direction == 3){
				return "Robot is turning left for " + to_string(duration) + " seconds at speed " + to_string(speed) + "\n";
			}
			else if(direction == 4){
				return "Robot is turning right for " + to_string(duration) + " seconds at speed " + to_string(speed) + "\n";
			}
		}
		else{
			return "Drive command failed";
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

	// for sending robot drive/sleep command
	CROW_ROUTE(app, "/telecommand/").methods(HTTPMethod::Put)
		([](const request& req, response& res) {


		});

	// for getting telemetry response
	CROW_ROUTE(app, "/telemetry/request/").methods(HTTPMethod::Get)
		([](const request& req, response& res) {


		});

	// for getting telemetry response
	CROW_ROUTE(app, "/drive").methods(HTTPMethod::POST)
		([&pktCount, &socket](const request& req, response& res) {
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
			respkt.PrintPkt();

			delete[] rx;
			string response = ResponseToString(reqpkt, respkt);
			res.code = 200;
			res.write(response);
			res.end();
		});

	app.port(25543).multithreaded().run();
	return 1;
}