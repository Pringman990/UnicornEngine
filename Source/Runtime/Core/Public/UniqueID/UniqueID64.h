#pragma once
#include <StandardTypes/StandardTypes.h>
#include "IDGenerator.h"
 
class UniqueID64
{
public:
	static const uint64 InvalidID = 0;
public:
	UniqueID64() : mID(0) {};
	
	/// <summary>
	/// Use with care as setting the id manually can lead to collision in IDs
	/// </summary>
	UniqueID64(uint64 ID) : mID(ID) {};

	~UniqueID64() {};

	static UniqueID64 GenerateNewID()
	{
		UniqueID64 newUniqueID(IDGenerator::Generator64ID());
		return newUniqueID;
	}

	void GetSplit32ID(uint32& HighOut, uint32& LowOut)
	{
		uint32 high = static_cast<uint32>(mID & 0xFFFFFFFF);
		uint32 low = static_cast<uint32>((mID >> 32) & 0xFFFFFFFF);

		HighOut = high;
		LowOut = low;
	}

	const uint64 Get64ID() const { return mID; }

public:
	UniqueID64 operator=(UniqueID64 other)
	{
		this->mID = other.mID;
		return *this;
	}

	bool operator==(const UniqueID64& other) const
	{
		return this->mID == other.mID;
	}

	bool operator!=(const UniqueID64& other) const
	{
		return this->mID != other.mID;
	}

private:
	uint64 mID;
};

namespace std {
	template <>
	struct hash<UniqueID64> {
		size_t operator()(const UniqueID64& id) const noexcept {
			// Use the underlying ID for hashing
			return hash<uint64_t>()(id.Get64ID());
		}
	};
}