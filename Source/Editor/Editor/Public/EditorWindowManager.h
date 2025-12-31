#pragma once
#include <EngineMinimal.h>
#include "EditorDefines.h"
#include "Editor.h"

class EditorWindow;

/*
* Engine subsystem
*/
class EditorWindowManager
{
	friend struct subsystem::SubsystemDescriptor;
public:
	enum class Stage
	{
		Static,
		Runtime
	};

public:

	EDITOR_API static EditorWindowManager* Instance();

	template<typename T>
	void RegisterWindow(UniqueID128 UUID, String Name)
	{
		mRegisteredWindows[UUID] = MakeOwned<T>(Editor::Instance());
		AddAndInitWindow(UUID, Name);
	}

	void UnregisterWindow(UniqueID128 UUID);

	void RenderActiveWindows();

	void ClearAllWindows();

	void InitAllRegisteredWindows();
	
private:
	EditorWindowManager();
	~EditorWindowManager();

#undef CreateWindow
	void AddAndInitWindow(UniqueID128 UUID, String Name);
private:
	UnorderedMap<UniqueID128, OwnedPtr<EditorWindow>> mRegisteredWindows;
	Vector<EditorWindow*> mActiveWindows;

	Vector<UniqueID128> mWindowsToInit;

	Stage mStage = Stage::Static;
};
#define REGISTER_EDITOR_WINDOW(TYPE, UUID)																			\
	namespace																										\
	{																												\
		struct TRUNCATE(Editor_Window_Register_, __LINE__)															\
		{																											\
			TRUNCATE(Editor_Window_Register_, __LINE__)()															\
			{																										\
				EditorWindowManager::Instance()->RegisterWindow<TYPE>(UniqueID128(UUID), STRINGIFYEXP(TYPE));					\
			}																										\
			~TRUNCATE(Editor_Window_Register_, __LINE__)()															\
			{																										\
				EditorWindowManager::Instance()->UnregisterWindow(UniqueID128(UUID));											\
			}																										\
		};																											\
	}																												\
	static const TRUNCATE(Editor_Window_Register_, __LINE__) TRUNCATE(Static_Editor_Window_Register_, __LINE__);		