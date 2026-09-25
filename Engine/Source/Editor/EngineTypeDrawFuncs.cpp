#include "EngineTypeDrawFuncs.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

float Wrap180(float degrees)
{
    degrees = std::fmod(degrees + 180.0f, 360.0f);

    if (degrees < 0.0f)
        degrees += 360.0f;

    return degrees - 180.0f;
}

glm::vec3 ToEulerDegrees(const glm::quat& q)
{
    glm::vec3 result = glm::vec3(glm::eulerAngles(q));

    result = glm::degrees(result);

    result.x = Wrap180(result.x);
    result.y = Wrap180(result.y);
    result.z = Wrap180(result.z);

    return result;
}

bool DrawInteger(const refl::TypeID& type, const String& displayName, void* obj)
{
    ImGuiDataType dataType = -1;

    auto& registry = refl::GetRegistry();
    if (type == registry.GetType<uint8>().id)
        dataType = ImGuiDataType_U8;
    if (type == registry.GetType<uint16>().id)
        dataType = ImGuiDataType_U16;
    if (type == registry.GetType<uint32>().id)
        dataType = ImGuiDataType_U32;
    if (type == registry.GetType<uint64>().id)
        dataType = ImGuiDataType_U64;
    if (type == registry.GetType<int8>().id)
        dataType = ImGuiDataType_S8;
    if (type == registry.GetType<int16>().id)
        dataType = ImGuiDataType_S16;
    if (type == registry.GetType<int32>().id)
        dataType = ImGuiDataType_S32;
    if (type == registry.GetType<int64>().id)
        dataType = ImGuiDataType_S64;
    if (type == registry.GetType<f32>().id)
        dataType = ImGuiDataType_Float;
    if (type == registry.GetType<f64>().id)
        dataType = ImGuiDataType_Double;

    ImGui::Text("%s: ", displayName.c_str());
    ImGui::SameLine();

    ImGui::PushID(displayName.c_str());
    bool changed = false;
    if (ImGui::DragScalar("##", dataType, obj, 0.01f))
    {
        changed = true;
    }
    ImGui::PopID();
    return changed;
}

void RegisterEngineTypeDrawFuncs(Editor& editor)
{
    editor.RegisterTypeDrawFunction<uint8>([](const refl::TypeID& type, const String& displayName, uint8* obj) -> bool
    {
        return DrawInteger(type, displayName, obj);
    });

    editor.RegisterTypeDrawFunction<uint16>([](const refl::TypeID& type, const String& displayName, uint16* obj) -> bool
    {
        return DrawInteger(type, displayName, obj);
    });

    editor.RegisterTypeDrawFunction<uint32>([](const refl::TypeID& type, const String& displayName, uint32* obj) -> bool
    {
        return DrawInteger(type, displayName, obj);
    });

    editor.RegisterTypeDrawFunction<uint64>([](const refl::TypeID& type, const String& displayName, uint64* obj) -> bool
    {
        return DrawInteger(type, displayName, obj);
    });

    editor.RegisterTypeDrawFunction<int8>([](const refl::TypeID& type, const String& displayName, int8* obj) -> bool
    {
        return DrawInteger(type, displayName, obj);
    });

    editor.RegisterTypeDrawFunction<int16>([](const refl::TypeID& type, const String& displayName, int16* obj) -> bool
    {
        return DrawInteger(type, displayName, obj);
    });

    editor.RegisterTypeDrawFunction<int32>([](const refl::TypeID& type, const String& displayName, int32* obj) -> bool
    {
        return DrawInteger(type, displayName, obj);
    });

    editor.RegisterTypeDrawFunction<int64>([](const refl::TypeID& type, const String& displayName, int64* obj) -> bool
    {
        return DrawInteger(type, displayName, obj);
    });

    editor.RegisterTypeDrawFunction<f32>([](const refl::TypeID& type, const String& displayName, f32* obj) -> bool
    {
        return DrawInteger(type, displayName, obj);
    });

    editor.RegisterTypeDrawFunction<f64>([](const refl::TypeID& type, const String& displayName, f64* obj) -> bool
    {
        return DrawInteger(type, displayName, obj);
    });

    editor.RegisterTypeDrawFunction<bool>([](const refl::TypeID& type, const String& displayName, bool* obj) -> bool
    {
        ImGui::Text("%s: ", displayName.c_str());
        ImGui::SameLine();
        bool changed = false;
        ImGui::PushID(displayName.c_str());
        if (ImGui::Checkbox("##", obj))
        {
            changed = true;
        }
        ImGui::PopID();
        return changed;
    });

    editor.RegisterTypeDrawFunction<String>([](const refl::TypeID& type, const String& displayName, String* obj) -> bool
    {
        ImGui::Text("%s: ", displayName.c_str());
        ImGui::SameLine();
        ImGui::PushID(displayName.c_str());
        bool changed = ImGui::InputText("##", obj);
        ImGui::PopID();
        return changed;
    });

    editor.RegisterTypeDrawFunction<const char*>([](const refl::TypeID& type, const String& displayName, const char** obj) -> bool
    {
        ImGui::PushID(displayName.c_str());
        ImGui::Text("%s: %s", displayName.c_str(), *obj);
        ImGui::PopID();
        return false;
    });

    editor.RegisterTypeDrawFunction<glm::vec2>([](const refl::TypeID& type, const String& displayName, glm::vec2* obj) -> bool
    {
        ImGui::PushID(displayName.c_str());
        const bool changed = ImGui::DragFloat2("##", &(*obj)[0], 0.01f);
        ImGui::PopID();
        return changed;
    });

    editor.RegisterTypeDrawFunction<glm::vec3>([](const refl::TypeID& type, const String& displayName, glm::vec3* obj) -> bool
    {
        ImGui::PushID(displayName.c_str());
        const bool changed = ImGui::DragFloat3("##", &(*obj)[0], 0.01f);
        ImGui::PopID();
        return changed;
    });

    editor.RegisterTypeDrawFunction<glm::vec4>([](const refl::TypeID& type, const String& displayName, glm::vec4* obj) -> bool
    {
        ImGui::PushID(displayName.c_str());
        const bool changed = ImGui::DragFloat4("##", &(*obj)[0], 0.01f);
        ImGui::PopID();
        return changed;
    });

    editor.RegisterTypeDrawFunction<glm::quat>([](const refl::TypeID& type, const String& displayName, glm::quat* obj) -> bool
   {
       ImGui::PushID(displayName.c_str());

        auto eulerDegrees = ToEulerDegrees(*obj);
        const bool changed = ImGui::DragFloat3("##", &eulerDegrees.x, 0.01f);
        if (changed)
        {
            *obj = glm::quat(glm::radians(eulerDegrees));
        }
       ImGui::PopID();
       return changed;
   });
}
