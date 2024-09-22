#include <iostream>
#include <Windows.h>
#include <Logger.h>

#include <MemoryDebugger.h>
#include <TUniquePtr.h>

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
	LOG_CORE_INFO("Windows Main Done Initilizing, Giving Control To GuardedMain");
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
	
	LOG_CORE_INFO("Windows Main Starting");
	
	_TRACK_MEMORY(true, false);

	int32_t result = GuardedMainWrapper();
	
	LOG_CORE_INFO("GuardedMain returned: {:0d}", result);
	LOG_CORE_INFO("Starting Windows Main Cleanup");
	
	Logger::Shutdown();
	
	_STOP_TRACK_MEMORY();

	return result;
}
