#pragma once
#include <StandardTypes/StandardTypes.h>
#include "IDGenerator.h"
 
class UniqueID
{
public:
	static const uint64 InvalidID = 0;
public:
	UniqueID() : mID(0) {};
	
	/// <summary>
	/// Use with care as setting the id manually can lead to collision in IDs
	/// </summary>
	UniqueID(uint64 aID) : mID(aID) {};

	~UniqueID() {};

	static UniqueID GenerateNewID()
	{
		UniqueID newUniqueID(IDGenerator::Generator64ID());
		return newUniqueID;
	}

	void GetSplit32ID(uint32& aHighOut, uint32& aLowOut)
	{
		uint32 high = static_cast<uint32>(mID & 0xFFFFFFFF);
		uint32 low = static_cast<uint32>((mID >> 32) & 0xFFFFFFFF);

		aHighOut = high;
		aLowOut = low;
	}

	const uint64 Get64ID() const { return mID; }

public:
	UniqueID operator=(UniqueID other)
	{
		this->mID = other.mID;
		return *this;
	}

	bool operator==(const UniqueID& other) const
	{
		return this->mID == other.mID;
	}

	bool operator!=(const UniqueID& other) const
	{
		return this->mID != other.mID;
	}

private:
	uint64 mID;
};

namespace std {
	template <>
	struct hash<UniqueID> {
		size_t operator()(const UniqueID& id) const noexcept {
			// Use the underlying ID for hashing
			return hash<uint64_t>()(id.Get64ID());
		}
	};
}