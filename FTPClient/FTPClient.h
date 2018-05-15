#pragma once

#include "resource.h"
#include <string>

class FTPClient
{
private:
	string user;
	string password;
	string hostIP;
	string request;
	string respone;
	
	bool login();
	bool connect();
	//
	void displayMessage();

public:
	FTPClient(){
		//connect to server: 
		//
	}
	void getCmd();


};