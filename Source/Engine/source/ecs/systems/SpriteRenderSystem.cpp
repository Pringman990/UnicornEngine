#include "EnginePch.h"
#include "SpriteRenderSystem.h"

#include <source/DX11.h>
#include <source/sprite/SpriteInstance.h>

#include "../components/Sprite.h"
#include "../components/Transform.h"

void ecs::engineSystem::SpriteRenderSystem(ecs::World& /*world*/, ecs::Entity /*entity*/, ecs::Transform& transform, ecs::Sprite& sprite)
{
	Engine::GetGraphicsEngine().SetSpriteRenderCameraAsActive();
	Engine::GetGraphicsEngine().GetDX11()->SetBlendState(dx::eAlphaBlend);
	
	transform.position.z = 1;
	sprite.sprite.GetTransform().SetPosition(transform.position);
	sprite.sprite.GetTransform().SetRotation(transform.rotation);
	sprite.sprite.GetTransform().SetScale(transform.scale);
	sprite.sprite.Draw();
	
	Engine::GetGraphicsEngine().GetDX11()->SetBlendState(dx::eDisableBlend);
}
