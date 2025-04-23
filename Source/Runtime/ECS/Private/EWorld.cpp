#include "pch.h"
#include "EWorld.h"

EWorld::EWorld()
{
}

EWorld::~EWorld()
{
}

void EWorld::ChangeArchetype(EEntity anEntity, Archetype* aFrom, Archetype* aTo)
{
	EntityLocation& entityLocation = mEntityToLocation[anEntity];
	uint32 entityIndex = entityLocation.indexInArray;
	
	EEntity entityToSwapWith = aFrom->entities.back();
	uint32 entityToSwapWithIndex = mEntityToLocation[entityToSwapWith].indexInArray;
	mEntityToLocation[entityToSwapWith].indexInArray = entityIndex;
	
	std::swap(aFrom->entities.back(), aFrom->entities[entityIndex]);
	aFrom->entities.pop_back();

	for (auto& [id, componentArray] : aFrom->components)
	{
		componentArray.Swap(entityIndex, entityToSwapWithIndex);

		void* newBlock = aTo->components[id].Allocate();
		componentArray.GetTypeInfo().move(newBlock, componentArray.Back());

		componentArray.Pop();
	}

	aTo->entities.push_back(anEntity);
	entityLocation.indexInArray = static_cast<uint32>(aTo->entities.size() - 1);
	entityLocation.archetype = aTo;
}
