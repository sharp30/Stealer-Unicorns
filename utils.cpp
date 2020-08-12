#include "utils.h"
#include <fstream>
#include <TlHelp32.h>

/*
This function casts from std::wstring to std::string
Input:std::wstring 
Ouptut:the input as std::wstring
*/
std::string fromWstringToString(std::wstring w)
{
    return string(w.begin(), w.end());
}



/*
The function performs the requested command in cmd and returns the result
Input:command - string: the requested command
Output:vector<string> - The output of the command
*/

vector<string> getCommandResult(string command)
{
    std::ifstream file;
    string line = "";
    //string output = "";
    int offest = 0;
    vector<string> outputLines;

    command += "> temp.txt";
    system(command.c_str());
    //open the file
    file.open("temp.txt");
    if (file.is_open())
    {
        while (!file.eof())
        {
            std::getline(file, line);
            {
                outputLines.push_back(line);
            }

        }
        file.close();
        std::remove("temp.txt");
    }
    return outputLines;
}

/*
This function transform map<int,std::string> to multimap <std::string,int>
Input:a map<int,std::string>
Output:input as multimap<int,std::string>
*/
std::multimap<std::string, int > fromMapToMultiMap(std::map<int,std::string> m)
{
    std::multimap<std::string, int> info;
    for (std::map<int, std::string>::iterator i = m.begin(); i != m.end(); i++)
    {
        info.insert(std::pair<string, int>(i->second, i->first));
    }
    return info;
}



std::map<int, string> getProcesses()
{

    HANDLE hProcsSnap; // the snap of the processes
    //HANDLE hProcs;
    PROCESSENTRY32 pe32; //processes entry
    //DWORD dwPriorityClass;
    hProcsSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    bool cond = true;

    std::map<int, std::string> procs;
    if (hProcsSnap == INVALID_HANDLE_VALUE)
    {
        return procs;
    }
    //set size of structure
    pe32.dwSize = sizeof(PROCESSENTRY32);

    //set first processs
    if (!Process32First(hProcsSnap, &pe32))
    {
        CloseHandle(hProcsSnap);
        //can't open processes 
        return procs;
    }
    while (cond)
    {
        std::string procName(fromWstringToString(pe32.szExeFile));
        int pid = pe32.th32ProcessID;
        procs.insert(std::pair<int, std::string>(pid, procName));
        cond = Process32Next(hProcsSnap, &pe32);

    }
    CloseHandle(hProcsSnap);
    return procs;
}

/*
This function returns the special key that created when pressing SHIFT + num
Input:key - the num on the top row
Output:The special key
*/
char getShiftSpecial(int key)
{
    switch (key)
    {
    case 1:
        return '!';
    case 2:
        return '@';
    case 3:
        return '#';
    case 4:
        return '$';
    case 5:
        return '%';
    case 6:
        return '^';
    case 7:
        return '&';
    case 8:
        return '*';
    case 9:
        return '(';
    case 0:
        return ')';
    default:
        return ' ';
    }
}

/*
This function sets a file to be hidden
Input:add - address of the file
Output:None
*/
void setFileHidden(std::string add)
{
    DWORD att = GetFileAttributesA(add.c_str());
    if (att != INVALID_FILE_ATTRIBUTES)
    {
        if ((att & FILE_ATTRIBUTE_HIDDEN) == 0)
        {

            SetFileAttributesA(add.c_str(), FILE_ATTRIBUTE_HIDDEN);

        }
    }
}