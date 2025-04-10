#include <iostream>
#include <Windows.h>
#include <memory>

extern int32_t GuardedMain();

void CreateConsoleWindow()
{
	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONIN$", "r", stdin);
	SetConsoleTitle(L"Console Window");
}

int32_t GuardedMainWrapper()
{
	return GuardedMain();
}

int32_t WINAPI WinMain(
	_In_ HINSTANCE hInInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ char* pCmdLine, 
	_In_ int32_t nCmdShow
)
{
	hInInstance;
	hPrevInstance;
	pCmdLine;
	nCmdShow;


#ifdef _DEBUG
	CreateConsoleWindow();
#endif // 

	int32_t result = GuardedMainWrapper();

	return result;
}
