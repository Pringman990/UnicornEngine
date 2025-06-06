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
	uint32 entityIndex = entityLocation.indexInArray;
	
	EEntity entityToSwapWith = From->entities.back();
	uint32 entityToSwapWithIndex = mEntityToLocation[entityToSwapWith].indexInArray;
	mEntityToLocation[entityToSwapWith].indexInArray = entityIndex;
	
	std::swap(From->entities.back(), From->entities[entityIndex]);
	From->entities.pop_back();

	for (auto& [id, componentArray] : From->components)
	{
		componentArray.Swap(entityIndex, entityToSwapWithIndex);

		void* newBlock = To->components[id].Allocate();
		componentArray.GetTypeInfo().move(newBlock, componentArray.Back());

		componentArray.Pop();
	}

	To->entities.push_back(Entity);
	entityLocation.indexInArray = static_cast<uint32>(To->entities.size() - 1);
	entityLocation.archetype = To;
}
