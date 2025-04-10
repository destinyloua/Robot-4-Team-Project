// CSCN72050 Final Project: Command and Control GUI 
// Destiny Louangsombath & Tyler Dao  

#define CROW_MAIN

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


int main()
{
	crow::SimpleApp app;

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
	CROW_ROUTE(app, "/connect").methods(HTTPMethod::Post)
		([](const request& req, response& res) {
		
			string ipAddress = "127.0.0.1";
			const int port = 23500;
			
			// TODO: logic to connect goes here
	
			res.code = 200;
			res.write("Connected to robot at 127.0.0.1:23500");
			res.end();
		});

	// for sending robot drive/sleep command
	CROW_ROUTE(app, "/telecommand/").methods(HTTPMethod::Put)
		([](const request& req, response& res) {


		});

	// for getting telemetry response
	CROW_ROUTE(app, "/telemetry/request/").methods(HTTPMethod::Get)
		([](const request& req, response& res) {


		});


	app.port(23500).multithreaded().run();
	return 1;
}