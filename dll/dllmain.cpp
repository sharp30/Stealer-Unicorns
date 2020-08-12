// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <string>
void changeBackground();
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )

{

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        changeBackground();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void changeBackground()
{
    HKEY hkey;
    std::string regPath = "Control Panel\\Desktop";
    char cPath[MAX_PATH];
    int len = GetModuleFileNameA(NULL, cPath, MAX_PATH);
    std::string path = cPath;
    path = path.substr(0, path.find_last_of('\\')+1) + "hacked.png";
    int return_value = SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0,(PVOID)path.c_str(), SPIF_UPDATEINIFILE);

}
