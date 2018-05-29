#pragma once
#define MAX_LENGTH 1000
#define MODE_ACTIVE 1
#define MODE_PASSIVE 0
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
	void action() { this->send(); this->receive(); this->displayMessage(); }
	//cmd
	void cmd_ls();
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
	void cmd_dir();
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
