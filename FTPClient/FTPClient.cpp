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

	this->dataPort = 0;
	this->cmdClient.Create();//auto choose client port, TCP type,  CAsyncSocket instance should listen for client activity on all network interfaces.
	this->dataClient.Create();
	hostIP = "127.0.0.1";
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