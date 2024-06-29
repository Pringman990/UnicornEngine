#pragma once

namespace utility
{

}

namespace ImGui
{
	/// <summary>
	/// Use std::string when you want to change the value in editor during runtime.
	/// Use const char* if you want to just display the value in the editor.
	/// 
	/// NOTE: In a built game the use of std::string or const char* wont matter.
	/// </summary>
	/// <param name="aMember"></param>
	/// <param name="someData"></param>
	extern void DisplayReflectedValues(const std::string& aTypeName, const reflection::MemberInfo& aMember, void*& someData);

	namespace internal
	{
		extern void DisplayVectorTypes(const std::string& aTypeName, const reflection::MemberInfo& aMember, char*& someData);
	}
}