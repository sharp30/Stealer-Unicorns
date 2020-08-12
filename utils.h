#pragma once
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <windows.h>

using std::string;
using std::vector;

std::multimap<std::string, int > fromMapToMultiMap(std::map<int, std::string> m);
std::string fromWstringToString(std::wstring w);
std::vector<std::string> getCommandResult(std::string command);
char getShiftSpecial(int key);
std::map< int,std::string> getProcesses();
void setFileHidden(std::string add);

