#include <iostream>
#include <Windows.h>
#include <memory>
#include <StandardTypes/StandardTypes.h>

extern int32 GuardedMain();

void CreateConsoleWindow()
{
	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONIN$", "r", stdin);
	SetConsoleTitle(L"Console Window");
}

int32 GuardedMainWrapper()
{
	return GuardedMain();
}

int32 WINAPI WinMain(
	_In_ HINSTANCE hInInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ char* pCmdLine, 
	_In_ int32 nCmdShow
)
{
	hInInstance;
	hPrevInstance;
	pCmdLine;
	nCmdShow;


#ifdef _DEBUG
	CreateConsoleWindow();
#endif // 

	int32 result = GuardedMainWrapper();

	return result;
}
