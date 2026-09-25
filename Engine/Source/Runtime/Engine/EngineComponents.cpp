#include "../pch.h"
#include "EngineComponents.h"

#include "../ECS/World.h"

void RegisterEngineComponents()
{
    refl::ClassRegistrator<Relationship>("Relationship", refl::TypeID("050c04a7-f199-4976-a491-955d11d50405"))
            .Category("Component")
            .Property("Parent", &Relationship::parent)
            .Finish();

    refl::ClassRegistrator<NameComponent>("Name", refl::TypeID("050c04a7-f199-4976-a491-955d11d50415"))
            .Category("Component")
            .Property("name", &NameComponent::name)
            .Finish();

    refl::ClassRegistrator<MeshRenderer>("Mesh Renderer", refl::TypeID("050c04a7-f199-4946-a491-955d11d50405"))
            .Category("Component")
            .Property("mesh", &MeshRenderer::model)
            .Finish();

    refl::ClassRegistrator<Camera>("Camera", refl::TypeID("050c14a7-f199-4976-a491-955d11d50405"))
            .Category("Component")
            .Finish();

    refl::ClassRegistrator<SpriteRenderer>("Sprite Renderer", refl::TypeID("150c04a7-f199-4976-a491-955d11d50405"))
            .Category("Component")
            .Property("mat", &SpriteRenderer::material)
            .Finish();

    refl::ClassRegistrator<CharacterController>("Character Controller", refl::TypeID("050c54a7-f199-4976-a491-955d11d50405"))
            .Category("Component")
            .Finish();

    refl::ClassRegistrator<BoxCollider2D>("BoxCollider2D", refl::TypeID("050c04b7-f199-4976-a491-955d11d50405"))
            .Category("Component")
            .Finish();

    refl::ClassRegistrator<Animator2D>("Animator2D", refl::TypeID("050c04a7-f119-4976-a491-955d11d50405"))
            .Category("Component")
            .Finish();

    refl::ClassRegistrator<Transform>("Transform", refl::TypeID("050c04a7-f199-4976-a491-952d11d50405"))
            .Category("Component")
            .Property("Position", &Transform::position)
            .Property("Rotation", &Transform::rotation)
            .Property("Scale", &Transform::scale)
            .Finish();
}
