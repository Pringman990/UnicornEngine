#pragma once
#include "EditorDefines.h"
#include <Reflection/ReflectionRegistry.h>

#include <Imgui.h>

class EditorViewRegistry
{
public:
	using EditorViewFunction = Func<void(void*, String)>;
public:

	EDITOR_API static EditorViewRegistry* Instance();

	template<typename T>
	void RegisterView(EditorViewFunction Fn)
	{
		ASSERT(Fn, "Function can't be nullptr");

		const refl::Type* type = refl::ReflectionRegistry::Instance()->GetOrCreateType<T>();

		auto it = mFunctions.find(type->uuid);
		if (it != mFunctions.end())
		{
			THROW("Type already registered with a function");
		}

		mFunctions.insert({ type->uuid, std::move(Fn) });
	}

	template<typename T, typename Fn>
	static EditorViewFunction WrapEditorView(Fn&& fn)
	{
		return [fn = std::forward<Fn>(fn)](void* data, String label)
			{
				fn(*static_cast<T*>(data), label);
			};
	}

	EDITOR_API EditorViewFunction GetFunction(UniqueID128 TypeID);

private:
	EDITOR_API EditorViewRegistry();
	EDITOR_API ~EditorViewRegistry();
private:
	UnorderedMap<UniqueID128, EditorViewFunction> mFunctions;
};

#define REGISTER_EDITOR_VIEW(TYPE, FN)																				\
	namespace																										\
	{																												\
		struct TRUNCATE(Editor_View_Register_, __LINE__)															\
		{																											\
			TRUNCATE(Editor_View_Register_, __LINE__)()																\
			{																										\
				EditorViewRegistry::Instance()->RegisterView<TYPE>(EditorViewRegistry::WrapEditorView<TYPE>(FN));	\
			}																										\
			~TRUNCATE(Editor_View_Register_, __LINE__)()															\
			{																										\
																													\
			}																										\
		};																											\
	}																												\
	static const TRUNCATE(Editor_View_Register_, __LINE__) TRUNCATE(Static_Editor_View_Register_, __LINE__);		