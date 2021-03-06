#include "stdafx.h"
#include "FTPClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
;
// The one and only application object
const vector<string> arrCmd = { "open","ls","put","get","mput","mget","cd","lcd","del",
							"mdel","mkdir","rmdir","pwd","pasv","quit","exit","clear","help","dir", "passive", "active"};
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
/*Constructor & Destructor*/
FTPClient::FTPClient()
{
	this->isConnected = 0;
	this->isLogged = 0;
	this->cmdClient.Create();
	this->mode = MODE_ACTIVE;
}

FTPClient::~FTPClient()
{
	this->cmdClient.Close();
}


/*Get user command Functions*/
string FTPClient::standardizedCMD(string cmd)
{
	//earse the extra whitespaces
	while (cmd[0] == ' ')
		cmd.erase(0, 1);
	while (cmd[cmd.size() - 1] == ' ')
		cmd.erase(cmd.size() - 1, 1);

	//all cmd will be lowered
	for (int i = 0; i < cmd.size(); i++)
	{
		if (isalpha(cmd[i]) && isupper(cmd[i]))
			cmd[i] = tolower(cmd[i]);
		if (i < cmd.size()- 1) 
			if (cmd[i] == ' '&& cmd[i + 1] == ' ')
			{
				cmd.erase(i, 1);
				i--;
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

bool FTPClient::checkIP()
{
	// The true IP will be formated x.x.x.x 
	//ex: 192.168.1.1
	for (auto c : argument[0])
		if (!isdigit(c) && c != '.')
			return 0;
	if (count(argument[0].begin(), argument[0].end(), '.') != 3)
		return 0;
	return 1;
}

void FTPClient::getCmd()
{
	string cmd = "", order = "";
LOOP:cout << "FTP >> ";
	getline(cin, cmd);
	if (cmd == "")
		goto LOOP;

	cmd = this->standardizedCMD(cmd);				
	order = cmd.substr(0, cmd.find_first_of(' '));	//get request

	if (order == "help")
	{
		this->cmd_help();
		getCmd();
	}

	this->getClauses(cmd);

	if (!isLogged||!isConnected)
	{
		if (!isConnected && !isLogged)
		{
			if (defineOrder(order) == Request_open) 
			{
				if (argument[0] == "localhost")
					argument[0] = IP_LOCALHOST;
				if (!checkIP())
				{
					cout << "IP incorrect!\n";
					goto LOOP;
				}
				this->hostIP = argument[0];
				this->connect();
			}
			else if (!isConnected) {
				cout << "Please open connection\n";
				goto LOOP;
			}
		}
		else if(!isConnected){
				cout << "Please open connection\n";
			goto LOOP;
		}
		else login();
	}
	else
	switch (defineOrder(order))
	{
	case Request_open: this->connect(); break;
	case Request_ls: this->cmd_ls();	break;
	case Request_put: this->cmd_put(); break;
	case Request_get: this->cmd_get(); break;
	case Request_mput: this->cmd_mput(); break;
	case Request_mget: this->cmd_mget(); break;
	case Request_cd: 
	{	if (argument[0] != order)
			this->cmd_cd(); 
		else 
			this->cmd_pwd(); 
	}break;
	case Request_lcd:
	{
		if (argument[0] != order)
			this->cmd_lcd();
		else
			cout << "Local directory now:" << getCurrentDirectory() << endl;
	}break;
	case Request_del: this->cmd_del(); break;
	case Request_mdel: this->cmd_mdel(); break;
	case Request_mkdir: this->cmd_mkdir(); break;
	case Request_rmdir: this->cmd_rmdir(); break;
	case Request_pwd: this->cmd_pwd(); break;
	case Request_pasv: this->cmd_pasv(); break;
	case Request_quit: 
	case Request_exit: this->cmd_quit(); break;
	case Request_clear: this->cmd_clear(); break;
	case Request_help: this->cmd_help(); break;
	case Request_dir: this->cmd_dir(); break;
	case Request_passive: this->cmd_passive(); break;
	case Request_active: this->cmd_active(); break;

	default: {cout << "error syntax\n"; goto LOOP; }
			 break;
	}
	goto LOOP;
}

/*Open port and connect to server*/
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

CSocket* FTPClient::openPassiveConnect()
{
	CSocket *dataClient = new CSocket();
	if (!dataClient->Create()){
		delete dataClient;
		return NULL;
	}

	int dataPort = 0;
	this->cmd_pasv();
	if (this->getServerCode() == 227)
	{
		dataPort = this->getDataPort();
		wstring wstrHost;
		wstrHost.assign(this->hostIP.begin(), this->hostIP.end());

		if (dataClient->Connect(wstrHost.c_str(), dataPort) == 0)
		{
			cout << "DataClient cannot connect to server " << dataPort << endl;
			delete dataClient;
			dataClient = NULL;
		}
	}
	else
	{
		cout << "Cannot connect to server " << dataPort << endl;
		delete dataClient;
		dataClient = NULL;
	}
	return dataClient;
}

CSocket* FTPClient::openActiveConnect()
{
	CSocket * dataClient = new CSocket();
	CString dataIP;
	unsigned int dataPort;
	short p0, p1;

	if (!dataClient->Create()) {
		delete dataClient;
		return NULL;
	}

	dataClient->GetSockName(dataIP, dataPort);//get current dataPort
	unsigned int tmp;
	this->cmdClient.GetSockName(dataIP, tmp);
	p1 = dataPort / 256;
	p0 = dataPort % 256;
	if (!dataClient->Listen(1)) {
		delete dataClient;
		return NULL;
	}

	// Convert a TCHAR string to a LPCSTR
	CT2CA pszConvertedAnsiString(dataIP);
	// construct a std::string using the LPCSTR input
	std::string ip(pszConvertedAnsiString);

	this->request = "PORT " + ip + "." + to_string(p1) + "." + to_string(p0) + "\r\n";
	this->action();

	if (this->getServerCode() != 200) {
		delete dataClient;
		return NULL;
	}

	return dataClient;
}


/*Init user access*/
bool FTPClient::login() {
	cout <<endl<< "USERNAME: ";	getline(cin, this->user);	this->cmd_user();
	cout << "PASSWORD: ";	
	char ch=_getch();
	password.clear();
	while (ch != ENTER) {//character 13 is enter
		if (ch == DELETE || ch == BACKSPACE)
		{
			if (password.size() > 0)
				password.erase(password.size() - 1, 1);
			if (ch == DELETE)
				ch = _getch();
		}
		else
		{
			this->password.push_back(ch);
			cout << '*';
		}
		ch = _getch();
	} cout << endl;
	this->cmd_pass();

	if (respone.substr(0, 3).compare(FailLoggin) == 0)
		login();
	else 
		isLogged = 1;
	this->getCmd();
	return true;
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

bool FTPClient::connect()
{
	wstring wstrHost;
	wstrHost.assign(hostIP.begin(), hostIP.end());
	if (cmdClient.Connect(wstrHost.c_str(), 21) != 0)
	{
		this->receive();
		this->isConnected = 1;

		this->displayMessage();
		this->login();
		return true;
	}
	else cout << "Can't connect to server.Make sure your server is online !\n";
	return false;
}

/**Non-transfer Commands*/

void FTPClient::cmd_pasv()
{
	request = "pasv";
	this->action();
}

void FTPClient::cmd_cd()
{
	cout << argument[0] << endl << endl;
	request = "CWD " + argument.at(0);
	this->action();
}

void FTPClient::cmd_lcd()
{
	CString newDir(argument[0].c_str());
	SetCurrentDirectory(newDir);
	if (argument[0] == getCurrentDirectory())
		cout << "Local directory now:" << getCurrentDirectory() << endl;
	else cout << "Local directory has not been changed !\n";
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

void FTPClient::cmd_quit()
{
	this->isLogged = 0; this->isConnected = 0;
	cmdClient.Close();
	cout << "Good bye !\n";
	Sleep(900);
	exit(0);
}

void FTPClient::cmd_help()
{
	cout << "Commands may be abbreviated.  Commands are:\n";
	for (int i = 0; i<arrCmd.size(); i++)
	{
		cout << setw(15) << left << arrCmd[i];
		if (i % 3 == 0)
			cout << endl;
	}
	cout << endl;
}

void FTPClient::cmd_passive()
{

	if (this->mode == MODE_ACTIVE)
	{
		this->mode = MODE_PASSIVE;
		cout << "Switch to PASSIVE mode successful.\n";
	}
	else
		cout << "You are currently in passive mode.\n";

}

void FTPClient::cmd_active()
{
	if (this->mode == MODE_PASSIVE) {
		this->mode = MODE_ACTIVE;
		cout << "Switch to ACTIVE mode successful.\n";
	}
	else
		cout << "You are currently in ACTIVE mode.\n";
}

void FTPClient::cmd_clear()
{
	system("cls");
	getCmd();
}


/*Transfer Commands*/
/*Listing methods*/
void FTPClient::cmd_list_core(const string command)
{
	CSocket*dataClient = this->openPort();
	if (dataClient && (this->getServerCode() == 227||this->getServerCode() == 200))
	{
		this->request = command+"\r\n";
		this->action();
		if (this->getServerCode() == 150 || this->getServerCode()==226)
		{	
			if (this->mode == MODE_ACTIVE)//active
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

void FTPClient::cmd_dir()
{
	this->cmd_list_core("LIST");
}

void FTPClient::cmd_ls()
{
	this->cmd_list_core("NLST");
}

/*Download Methods*/
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
				if (this->mode == MODE_ACTIVE)
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


/*Upload Methods*/
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
				if (this->mode == MODE_ACTIVE)//active
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
	this->cmd_put_core(argument[0]);
	if (this->getServerCode() != 226)
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



/*Support Functions*/
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

void FTPClient::action()
{
	this->send();
	this->receive();
	this->displayMessage();
}

void FTPClient::send()
{
	this->cmdClient.Send(request.c_str(), request.length() + 1, 0);
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
	if (respone[0] == isBreakCNN || respone[2] == isBreakCNN)
	{
		cout << "Connection is broken !\n";
		cmdClient.Close();
	}
	else 	cout << this->respone;
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

string FTPClient::getCurrentDirectory()
{
	string path = "";
	TCHAR Buffer[MAX_PATH];
	DWORD dwRet;
	dwRet = GetCurrentDirectory(MAX_PATH, Buffer);
	for (int i = 0; i<dwRet; i++)
		path+=(char)*(Buffer + i);
	return path;
}

