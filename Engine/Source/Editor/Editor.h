#pragma once
#include "Core/ReflectionRegistry.h"
#include "ECS/EntityManager.h"

template<typename T>
using EditorDrawTypeFunc = Func<bool(const refl::TypeID& type, const String& displayName, T* obj)>;

class Editor
{
    using EditorDrawTypeFuncRaw = Func<bool(const refl::TypeID& type, const String& displayName, void* obj)>;

    INIT_SERVICE(Editor)
public:
    Editor();
    ~Editor();

    NODISC const List<Entity>& GetSelectedEntities() const {return mSelectedEntities;};
    void SelectEntity(Entity entity) { mSelectedEntities.push_back(entity); };
    void ClearSelectedEntities() { mSelectedEntities.clear(); };
    bool IsEntitySelected(Entity entity)
    {
        const auto it = std::ranges::find(mSelectedEntities, entity);
        return it != mSelectedEntities.end();
    };

    template<typename T>
    void RegisterTypeDrawFunction(EditorDrawTypeFunc<T> func)
    {
        EditorDrawTypeFuncRaw rawFunc = [func](const refl::TypeID& type, const String& displayName, void* obj) -> bool
        {
            return func(type, displayName, static_cast<T*>(obj));
        };

        mTypeDrawFuncs.insert({refl::GetRegistry().GetType<T>().id, rawFunc});
    }

    bool DrawType(const refl::TypeID& type, const String& displayName, void* obj)
    {
        if (!type.IsValid())
            return false;

        auto it = mTypeDrawFuncs.find(type);
        if (it == mTypeDrawFuncs.end())
            return false;

        return it->second(type, displayName, obj);
    }

    bool HasTypeDrawer(const refl::TypeID& type)
    {
        auto it = mTypeDrawFuncs.find(type);
        if (it == mTypeDrawFuncs.end())
            return false;

        return true;
    }

private:
    List<Entity> mSelectedEntities;

    UnorderedMap<refl::TypeID, EditorDrawTypeFuncRaw> mTypeDrawFuncs;
};
