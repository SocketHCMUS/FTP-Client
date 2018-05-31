#pragma once
#define MAX_LENGTH 1000
#define MODE_ACTIVE 1
#define MODE_PASSIVE 0
#define IP_LOCALHOST "127.0.0.1"
#define Request_open 0
#define Request_ls 1
#define Request_put 2
#define Request_get 3
#define Request_mput 4
#define Request_mget 5
#define Request_cd 6
#define Request_lcd 7
#define Request_del 8
#define Request_mdel 9
#define Request_mkdir 10
#define Request_rmdir 11
#define Request_pwd 12
#define Request_pasv 13
#define Request_quit 14
#define Request_exit 15
#define Request_clear 16
#define Request_help 17
#define Request_dir 18
#define Request_passive 19
#define Request_active 20

#define ENTER 13
#define DELETE -32
#define BACKSPACE 8

#define FailLoggin "530"
#define isBreakCNN -51

#include "resource.h"
#include <string>
#include <fstream>
#include <afxsock.h>
#include <string.h>
#include <vector>
#include <conio.h>
#include <Windows.h>
#include <iomanip> 

using namespace std;

class FTPClient
{
private:
	CSocket cmdClient;
	std::string user;

	string password;
	string hostIP;
	string request;
	string respone;
	vector<string> argument;

	bool mode;//0: passive, 1:active
	bool isLogged;
	bool isConnected;
public:
	bool login();
	bool connect();
	//
	void send();
	int receive();
	void displayMessage();
	void action() 
	{ 
		this->send(); 
		this->receive(); 
		this->displayMessage(); 
	}
	//cmd

	void FTPClient::cmd_list_core(const string command);
	void cmd_ls();
	void cmd_dir();
	void cmd_pwd();
	void cmd_cd();
	void cmd_lcd();
	void cmd_get();
	void cmd_get_core(const string filename);
	void cmd_put();
	bool cmd_put_core(const string filename);
	void cmd_mget();
	void cmd_mput();
	void cmd_del();
	void cmd_mdel();
	void cmd_mkdir();
	void cmd_rmdir();
	void cmd_pasv();
	void cmd_quit();
	void cmd_user();
	void cmd_pass();
	void cmd_clear() { system("cls"); getCmd(); }
	void cmd_help();
	
	void cmd_passive();
	void cmd_active();
	//support function
	string standardizedCMD(string);
	int defineOrder(string);
	void getClauses(string cmd);
	string getCurrentDirectory();
	bool checkIP();

	FTPClient();
	//FTPClient(string mHostIP, int dataPort);
	~FTPClient();
	void getCmd();
	int getDataPort();
	int getServerCode();

	///active method
	CSocket* openPort();
	CSocket* openPassiveConnect();
	CSocket* openActiveConnect();

};
CSocket* FTPClient::openPort()
{
	CSocket*dataClient = NULL;
	if (this->mode == MODE_PASSIVE)
	{
		dataClient = openPassiveConnect();
	}
	else
	{
		dataClient = openActiveConnect();
	}
	return dataClient;
}
