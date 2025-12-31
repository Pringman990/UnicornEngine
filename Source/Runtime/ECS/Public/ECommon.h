#pragma once
#include <Core.h>
#include <bitset>
#include "EComponentAllocator.h"

class EWorld;
#undef max

using EEntity = UniqueID128;

//struct EEntityHandle
//{
//	EEntityHandle()
//	{
//		generation = 0;
//		index = UINT32_MAX;
//	}
//
//	EEntityHandle(uint64 ID)
//	{
//		generation = uint32(ID & 0xFFFFFFFF);
//		index = uint32(ID >> 32);
//	}
//
//	uint32 generation;
//	uint32 index;
//
//	uint64 ToUint64() const { return (uint64(generation) << 32) | index; }
//};

//#define E_MAX_CHUNK_SIZE 64
#define E_MAX_COMPONENTS_PER_ENTITY 256

using ESignature = std::bitset<E_MAX_COMPONENTS_PER_ENTITY>;

using ESystemFunction = Func<void(EWorld&)>;

enum EPipeline
{
	ESystemLoad = 0,
	ESystemPostLoad = 1,
	ESystemPreUpdate = 2,
	ESystemUpdate = 3,
	ESystemPostUpdate = 5,
	ESystemCount = ESystemPostUpdate + 1
};

struct ESystem final
{
	EPipeline pipeline = ESystemUpdate;
	uint32 executionPhase = 0;
	String name = "";
	ESignature signature = 0;
	ESystemFunction function;
};

struct ComponentStore
{
	EComponentAllocator allocator;
	Vector<EEntity> entities;
	UnorderedMap<EEntity, uint32> indicies;
	UnorderedMap<uint32, EEntity> indexToEntity;

	bool HasEntity(EEntity Entity) const
	{
		return indicies.contains(Entity);
	}

	template<typename T>
	T* Get(uint32 Index)
	{
		return reinterpret_cast<T*>(allocator.Get(Index));
	}
};


using EPipelineSystemMap = UnorderedMap<EPipeline, Vector<ESystem>>;
using ENameSystemMap = UnorderedMap<String, ESystem>;
//
//struct Archetype
//{
//	Archetype(ESignature Signature) : signature(Signature) {};
//
//	const ESignature signature;
//	UnorderedMap<size_t, EComponentAllocator> components;
//	Vector<EEntity> entities;
//};
//
//struct EntityLocation
//{
//	Archetype* archetype;
//	uint32 allocatorIndex;
//};