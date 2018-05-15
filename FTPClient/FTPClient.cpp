// FTPClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FTPClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			if (AfxSocketInit() == FALSE){
				cout << "Khong the khoi tao Socket Libraray";
				return FALSE;
			}
			// TODO: code your application's behavior here.
			FTPClient client;
			client.connect();
			
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}


//-----------------
FTPClient::FTPClient(){

	//this->dataPort = 0;
	this->cmdClient.Create();//auto choose client port, TCP type,  CAsyncSocket instance should listen for client activity on all network interfaces.
	//this->dataClient.Create();
	//hostIP = "127.0.0.1";
}

FTPClient::FTPClient(string mHostIP, int dataPort){

	this->hostIP = mHostIP;
	this->cmdClient.Create();//auto choose client port, TCP type,  CAsyncSocket instance should listen for client activity on all network interfaces.
	wstring wstrHost;
	wstrHost.assign(hostIP.begin(), hostIP.end());
	if (cmdClient.Connect(wstrHost.c_str(), dataPort) == 0)
		cout << "-Fatal error!!";
}
FTPClient::~FTPClient()
{
	this->cmdClient.Close();
}
bool FTPClient::connect()
{
	wstring wstrHost;
	wstrHost.assign(hostIP.begin(), hostIP.end());
	if (cmdClient.Connect(wstrHost.c_str(), 21) != 0)
		return true;
	return false;
}


void FTPClient::send()
{
	this->cmdClient.Send(request.c_str(), request.length(), 0);
}
void FTPClient::receive()
{
	respone.clear();
	char *temp = new char[MAX_LENGTH];
	int len = this->cmdClient.Receive(temp, MAX_LENGTH, 0);
	respone = string(temp).substr(0, len);

}
void FTPClient::displayMessage()
{
	cout << this->respone << endl;
}

void FTPClient::cmd_user()
{
	request = "USER " + user;
	this->action();
}
void FTPClient::cmd_pass()
{
	request = "PASS " + password;
	this->action();
}
void FTPClient::cmd_ls()
{
	this->cmd_pasv();
	FTPClient dataClient(this->hostIP, this->getDataPort());
	dataClient.request = "LIST\r\n";
	dataClient.action();
}
void FTPClient::cmd_pwd()
{
	request = "PWD";
	this->action();
}
void FTPClient::cmd_mkdir()
{
	request = "MKD " + argument.at(0);
	this->action();
}
void FTPClient::cmd_rmdir()
{
	request = "RMD " + argument.at(0);
	this->action();
}

void FTPClient::cmd_pasv()
{
	request = "pasv";
	this->action();
}

int FTPClient::getDataPort()
{
	string str = this->respone;
	int pos1 = 0; int count = 0;
	for (; pos1 < str.length() && count < 4; pos1++)
	{
		if (str.at(pos1) == ',')
			count++;
	}

	int pos2 = str.find_last_of(',');
	string a_str = str.substr(pos1, pos2 - pos1);
	pos1 = str.find_first_of(')');
	string b_str = str.substr(pos2 + 1, pos1 - pos2);
	int a = atoi(a_str.c_str());
	int b = atoi(b_str.c_str());
	return a * 256 + b;
}
