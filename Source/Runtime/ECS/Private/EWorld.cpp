#include "pch.h"
#include "EWorld.h"

EWorld::EWorld()
{
}

EWorld::~EWorld()
{
}

void EWorld::ChangeArchetype(EEntity Entity, Archetype* From, Archetype* To)
{
	EntityLocation& entityLocation = mEntityToLocation[Entity];
	uint32 entityIndex = entityLocation.allocatorIndex;
	
	EEntity entityToSwapWith = From->entities.back();
	uint32 entityToSwapWithIndex = mEntityToLocation[entityToSwapWith].allocatorIndex;
	mEntityToLocation[entityToSwapWith].allocatorIndex = entityIndex;
	
	std::swap(From->entities.back(), From->entities[entityIndex]);
	From->entities.pop_back();

	for (auto& [typeHash, componentArray] : From->components)
	{
		componentArray.Swap(entityIndex, entityToSwapWithIndex);

		void* newBlock = To->components[typeHash].Allocate();
		const auto& typeInfo = ReflectionRegistry::Get()->GetInfo(typeHash);
		typeInfo.moveFunc(newBlock, componentArray.Back());

		componentArray.Pop();
	}

	To->entities.push_back(Entity);
	entityLocation.allocatorIndex = static_cast<uint32>(To->entities.size() - 1);
	entityLocation.archetype = To;
}
