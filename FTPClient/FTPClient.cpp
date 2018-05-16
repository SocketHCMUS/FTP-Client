// FTPClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FTPClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object
vector<string> arrCmd = { "open","ls","put","get","mput","mget","cd","lcd","del",
							"mdel","mkdir","rmdir","pwd","passive","quit","exit","clear","help"};

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			if (AfxSocketInit() == FALSE){
				cout << "Khong the khoi tao Socket Libraray";
				return FALSE;
			}
			FTPClient client;
			client.getCmd();
		}
	}
	else
	{
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}

//----------------------------------------------------------------------------
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
	for (int i = 0; i < arrCmd.size(); i++)
		if (arrCmd[i].compare(order) == 0)
			return i;
}
void FTPClient::getClauses(string cmd) 
{
	cmd.erase(0, cmd.find_first_of(' ') + 1);
	this->argument.clear();
	while (cmd.size() > 0)
	{
		argument.push_back(cmd.substr(0, cmd.find_first_of(' ')));
		if(cmd.find(' ')!=std::string::npos)
			cmd.erase(0, cmd.find_first_of(' ') + 1);
		else break;
	}
}
void FTPClient::getCmd()
{
	string cmd = "", order = "";
LOOP:cout << "FTP >> ";
	getline(cin, cmd);
	if (cmd == "")
		goto LOOP;
	cmd = this->standardizedCMD(cmd);
	order = cmd.substr(0, cmd.find_first_of(' '));
	this->getClauses(cmd);
	if (!isLogged||!isConnected)
	{
		if (!isConnected&& !isLogged)
		{
			if (defineOrder(order) == 0) 
			{
				string clause = argument[0];
				if (clause == "localhost")
					clause = "127.0.0.1";
				this->hostIP = clause;
				this->connect();
			}
			else if (!isConnected) {
				cout << "Please open connection\n";
				goto LOOP;
			}
		}
		else if(!isConnected)		{
			cout << "Please open connection\n";
			goto LOOP;
		}
		else login();
	}
	else
	switch (defineOrder(order))
	{
	case 1: this->cmd_ls();	break;
	case 2: this->cmd_put(); break;
	case 3: this->cmd_get(); break;
	case 4: this->cmd_mput(); break;
	case 5: this->cmd_mget(); break;
	case 6: this->cmd_cd(); break;
	case 7: this->cmd_lcd(); break;
	case 8: this->cmd_del(); break;
	case 9: this->cmd_mdel(); break;
	case 10: this->cmd_mkdir(); break;
	case 11: this->cmd_rmdir(); break;
	case 12: this->cmd_pwd(); break;
	case 13: this->cmd_pasv(); break;
	case 14: 
	case 15: this->cmd_quit(); break;
	case 16: this->cmd_clear(); break;
	case 17: this->cmd_help(); break;
	default: {cout << "error syntax\n"; goto LOOP; }
			 break;
	}
	goto LOOP;
}
//{ 0."open",1."ls",2."put",3."get",4."mput",5."mget",6."cd",7."lcd",8."delete"
//,9."mdelete",10."mkdir",11."rmdir",12."pwd",13."passive",14."quit",15."exit,16.clear",17."help"};
//-----------------
FTPClient::FTPClient()
{
	this->isConnected = 0;
	this->isLogged = 0;
	this->cmdClient.Create();
	this->mode = 1;//active
}

//FTPClient::FTPClient(string mHostIP, int dataPort)
//{
//	this->hostIP = mHostIP;
//	this->cmdClient.Create();
//	wstring wstrHost;
//	wstrHost.assign(hostIP.begin(), hostIP.end());
//	if (cmdClient.Connect(wstrHost.c_str(), dataPort) == 0)
//		cout << "-Fatal error: Cannot connect to port "<<dataPort;
//}

CSocket* FTPClient::openPassiveConnect()
{
	CSocket *dataClient = new CSocket();
	if (!dataClient->Create()){
		delete dataClient;
		return NULL;
	}

	this->cmd_pasv();
	int dataPort = this->getDataPort();

	wstring wstrHost;
	wstrHost.assign(this->hostIP.begin(), this->hostIP.end());
	
	if (dataClient->Connect(wstrHost.c_str(), dataPort) == 0)
	{
		cout << "Cannot connect to port " << dataPort;
		delete dataClient;
		return NULL;
	}
	return dataClient;
}

CSocket* FTPClient::openActiveConnect()
{
	CSocket * dataClient = new CSocket();
	CString hostIP;
	unsigned int dataPort;
	short p0, p1;

	if (!dataClient->Create()) {
		delete dataClient;
		return NULL;
	}

	dataClient->GetSockName(hostIP, dataPort);//get current dataPort
	p1 = dataPort / 256;
	p0 = dataPort % 256;

	if (!dataClient->Listen(1)) {
		delete dataClient;
		return NULL;
	}

	this->request = "PORT "+this->hostIP +"."+ to_string(p1) + "." + to_string(p0) + "\r\n";
	this->action();

	if (this->getServerCode() != 200) {
		delete dataClient;
		return NULL;
	}

	return dataClient;
}
FTPClient::~FTPClient()
{
	this->cmdClient.Close();
	//cout << "Close connection.";
}
bool FTPClient::login() {
	cout << endl;
	cout << "USERNAME: ";	getline(cin, this->user);
	this->cmd_user();
	cout << "PASSWORD: ";	getline(cin, this->password);
	this->cmd_pass();
	if (respone.substr(0, 3).compare("530") == 0)
		login();
	else isLogged = 1;
	this->getCmd();
	return true;
}
bool FTPClient::connect()
{
	wstring wstrHost;
	wstrHost.assign(hostIP.begin(), hostIP.end());
	if (cmdClient.Connect(wstrHost.c_str(), 21) != 0)
	{
		this->receive();
		cout << respone;
		if (respone.substr(0, 9).compare("Connected") == 0||respone.substr(0,3).compare("220")==0)
			this->isConnected = 1;
		this->displayMessage();
		this->login();
		return true;
	}
	return false;
}


void FTPClient::send()
{
	this->cmdClient.Send(request.c_str(), request.length()+1, 0);
}
int FTPClient::receive()
{
	respone.clear();
	char *temp = new char[MAX_LENGTH];
	int len = this->cmdClient.Receive(temp, MAX_LENGTH, 0);
	respone = string(temp).substr(0, len);
	delete temp;
	return len;
}
void FTPClient::displayMessage()
{
	cout << this->respone;
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
	if (this->getServerCode() == 530)
	{
		cout << "Authentication failed. Try Again!" << endl;
		this->login();
	}
}

void FTPClient::cmd_pasv()
{
	request = "pasv";
	this->action();
}
void FTPClient::cmd_ls()
{
	CSocket*dataClient = this->openPort();
	if (dataClient && (this->getServerCode() == 227||this->getServerCode() == 200))
	{
		this->request = "NLST\r\n";
		this->action();
		if (this->getServerCode() == 150 || this->getServerCode()==226)
		{	
			if (this->mode == 1)//active
			{
				if (dataClient->Listen(1) == false)
				{
					cout << "Cannot listen on this port" << endl;
					return;
				}
				CSocket Connector;
				if (dataClient->Accept(Connector))
				{
					char *temp = new char[MAX_LENGTH];
					int len = Connector.Receive(temp, MAX_LENGTH, 0);
					temp[len] = 0;
					cout << temp;
					delete temp;
				}
				else
				{
					cout << "Cannot Accept this connector" << endl;
				}
			}
			else
			{
				char *temp = new char[MAX_LENGTH];
				int len = dataClient->Receive(temp, MAX_LENGTH, 0);
				temp[len] = 0;
				cout << temp;
				delete temp;
			}
			
			if (this->getServerCode() != 226)
			{
				this->receive();
				this->displayMessage();
			}
		}
	}
	else
		cout << "Command Failed. Try again!"<<endl;
	delete dataClient;
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

void FTPClient::cmd_cd()
{
	request = "CWD " + argument.at(0);
	this->action();
}
void FTPClient::cmd_del()
{
	request = "DELE " + argument.at(0);
	this->action();
}
void FTPClient::cmd_mdel()
{
	for (int i = 0; i < this->argument.size(); i++)
	{
		cout << "Confirm to delete \"" + argument.at(i)+"\" ?\n";
		char ch = getch();
		if (ch == 'n' || ch == 'N')
		{
			cout << endl;
			continue;
		}
		request = "DELE " + argument.at(i);
		this->action();
	}
}
void FTPClient::cmd_get_core(const string filename)
{
	CSocket*dataClient = openPort();
	
	if (dataClient && this->getServerCode() == 227|| this->getServerCode()==200)//200: command success
	{
		this->request = "RETR " + filename + "\r\n";
		this->action();
		if (this->getServerCode() == 150 )//150: pasv port, 
		{

			ofstream os = ofstream(filename, ios::binary);
			if (os.is_open())
			{
				CSocket *culi = dataClient; //defautly culi works in passive mode
				CSocket Connector;
				if (this->mode == 1)//active
				{
					if (dataClient->Listen(1) == false)
					{
						cout << "Cannot listen on this port" << endl;
						return;
					}
					
					if (dataClient->Accept(Connector))
						culi = &Connector;	//now culi works in acive mode
					else
						cout << "Cannot Accept this connector" << endl;
				}
				//
				int len;
				char *temp = new char[MAX_LENGTH];
				do
				{
					memset(temp, 0, MAX_LENGTH);
					len = culi->Receive(temp, MAX_LENGTH, 0);
					os.write(temp, len);
				} while (len > 0);
				os.close();
				delete temp;

			if (this->getServerCode() != 226)
				{
					this->receive();
					this->displayMessage();
				}
			}
			else
				cout << filename << ": File not found." << endl;
		}
	}
	else
	{
		cout << "Command get failed. Try again!" << endl;
	}
	delete dataClient;
}
void FTPClient::cmd_get()
{
	this->cmd_get_core(this->argument.at(0));
}

void FTPClient::cmd_mget()
{
	for (int i = 0; i < this->argument.size(); i++)
	{
		this->cmd_get_core(this->argument.at(i));
	}
}

void FTPClient::cmd_lcd()
{
	CString newDir(argument[0].c_str());
	cout << newDir.GetString();
	SetCurrentDirectory(newDir);
}

bool FTPClient::cmd_put_core(const string filename)
{
	bool res = true;
	ifstream fin;
	fin.open(filename.c_str(), ios::binary);
	if (fin.is_open())
	{
		CSocket*dataClient = openPort();
		if (dataClient && this->getServerCode() == 227 || this->getServerCode() == 200)//200: command success
		{
			this->request = "STOR " + filename + "\r\n";
			this->action();//150
			if (this->getServerCode() == 150)
			{
				CSocket * culi = dataClient;//culi defaut transfer as passive mode
				CSocket Connector;
				streamsize len;
				char*data = new char[MAX_LENGTH];
				if (this->mode == 1)//active
				{
					if (dataClient->Listen(1) == false)
					{
						cout << "Cannot listen on this port" << endl;
						return false;
					}
					
					if (dataClient->Accept(Connector))
						culi = &Connector;
					else
					{
						cout << "Cannot Accept this connector" << endl;
						return false;
					}
				}
				//
				while (!fin.eof())
				{
					memset(data, 0, MAX_LENGTH);
					fin.read(data, MAX_LENGTH);
					len = fin.gcount();
					culi->Send(data, len);
				}
				fin.close();
				delete data;
			}
		}
		else
		{
			cout << "Command Failed. Try again!" << endl;
			res = false;
		}
		delete dataClient;
	}
	else
	{
		cout << filename << ": File not found" << endl;
		res = false;
	}
	return res;
	
}
void FTPClient::cmd_put()
{
	bool res =this->cmd_put_core(this->argument.at(0));
	if (this->getServerCode()!=226 && res==1)
	{
		this->receive();
		this->displayMessage();
	}
}
void FTPClient::cmd_mput()
{
	bool res;
	for (int i = 0; i < this->argument.size(); i++)
	{
		cout << "Upload \"" + this->argument.at(i) + "\"? "<<endl;
		char ch = getch();
		if (ch == 'n' || ch == 'N')
		{
			cout << endl;
			continue;
		}
		res = this->cmd_put_core(this->argument.at(i));
		if (this->getServerCode() != 226 && res == 1)
		{
			this->receive();
			this->displayMessage();
		}
	}
	
}
void FTPClient::cmd_quit()
{
	this->isLogged = 0; this->isConnected = 0;
	cmdClient.Close();
	exit(0);
}
void FTPClient::cmd_help()
{
	cout << "Commands may be abbreviated.  Commands are:";
	for (int i=0;i<arrCmd.size();i++)
	{
		cout << setw(15) <<left<< arrCmd[i];
		if (i % 3 == 0)
			cout << endl;
	}
	cout << endl;
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

int FTPClient::getServerCode()
{
	string str = "";
	int pos = respone.find_first_of('\n');
	if (pos == respone.size() - 1)
		str = respone.substr(0, respone.find_first_of(' '));
	else
	{
		str = respone.substr(pos + 1, respone.size());
		str = str.substr(0, str.find_first_of(' '));
	}

	return atoi(str.c_str());
}