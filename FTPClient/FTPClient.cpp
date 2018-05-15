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
			// TODO: code your application's behavior here.
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
//--------------------
string FTPClient::standardizedCMD(string cmd)
{
	while (cmd[0] == ' ')
		cmd.erase(0, 1);
	while (cmd[cmd.size() - 1] == ' ')
		cmd.erase(cmd.size() - 1, 1);

	for (int i = 0; i < cmd.size(); i++)
	{
		if (isalpha(cmd[i]) && isupper(cmd[i]))
			cmd[i] = tolower(cmd[i]);
		if (i < cmd.size()- 1) {
			if (cmd[i] == ' '&& cmd[i + 1] == ' ')
			{
				cmd.erase(i, 1);
				i--;
			}
		}
	}
	return cmd;
}
int FTPClient::defineOrder(string order)
{
	if (order.compare("open"))
		return 1;
}
void FTPClient::getCmd()
{
	string cmd = "", order = "", clause = "";
	LOOP:cout << "FTP>>";
	getline(cin, cmd);
	cmd =this->standardizedCMD(cmd);
	order=cmd.substr(0, cmd.find_first_of(' '));

	switch (defineOrder(order))
	{
	case 1: {
		clause = cmd.substr(cmd.find_first_of(' ') + 1, cmd.size() - cmd.find_first_of(' ') - 1);
		if (clause == "localhost")
			clause = "127.0.0.1";
		this->hostIP = clause;
		this->connect();
	}break;
	default: {cout << "error syntax\n"; goto LOOP; }
		break;
	}
}