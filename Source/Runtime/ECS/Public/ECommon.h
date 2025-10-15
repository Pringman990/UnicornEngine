#pragma once
#include <Core.h>
#include <bitset>
#include "EComponentAllocator.h"

class EWorld;
#undef max

using EEntity = uint32;

#define E_MAX_CHUNK_SIZE 64
#define E_MAX_COMPONENTS_PER_ENTITY 256

using ESignature = std::bitset<E_MAX_COMPONENTS_PER_ENTITY>;

using ESystemFunction = Func<void(EWorld&)>;

enum EPipeline
{
	ESystemLoad,
	ESystemPostLoad,
	ESystemPreUpdate,
	ESystemUpdate,
	ESystemPostUpdate,
	ESystemCount
};

struct ESystem final
{
	EPipeline pipeline = ESystemUpdate;
	String name = "";
	ESignature signature = 0;
	ESystemFunction function;
};

using EPipelineSystemMap = UnorderedMap<EPipeline, Vector<ESystem>>;
using ENameSystemMap = UnorderedMap<String, ESystem>;

struct Archetype
{
	Archetype(ESignature Signature) : signature(Signature) {};

	const ESignature signature;
	UnorderedMap<size_t, EComponentAllocator> components;
	Vector<EEntity> entities;
};

struct EntityLocation
{
	Archetype* archetype;
	uint32 allocatorIndex;
};