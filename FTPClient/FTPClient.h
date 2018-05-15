#pragma once

#include "resource.h"
#include <string>
using namespace std;
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

	//support function
	string standardizedCMD(string);
	int defineOrder(string);
public:
	FTPClient(){
		//connect to server: 
		//
	}
	void getCmd();


};