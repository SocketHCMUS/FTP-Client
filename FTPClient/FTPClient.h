#pragma once
#define MAX_LENGTH 1000
#include "resource.h"
#include <string>
#include <afxsock.h>
#include <string.h>
using namespace std;

class FTPClient
{
private:
	CSocket cmdClient;
	CSocket dataClient;
	int		dataPort;
	string user;
	string password;
	string hostIP;
	string request;
	string respone;
public:	
	bool login();
	bool connect();
	//
	void send();
	void receive();
	void displayMessage();

//public:
	FTPClient();
	void getCmd();


};