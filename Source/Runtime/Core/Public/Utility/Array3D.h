#pragma once
#include <algorithm>

template<typename Type>
class Array3D
{
public:
	Array3D() : mSizeX(0), mSizeY(0), mSizeZ(0), mData(nullptr) {};
	
	Array3D(uint32 aSizeX, uint32 aSizeY, uint32 aSizeZ)
		: mSizeX(aSizeX), mSizeY(aSizeY), mSizeZ(aSizeZ), mData(new Type[aSizeX * aSizeY * aSizeZ]) {};
	
	Array3D(uint32 aSizeX, uint32 aSizeY, uint32 aSizeZ, Type aDefualt)
		: mSizeX(aSizeX), mSizeY(aSizeY), mSizeZ(aSizeZ), mData(new Type[aSizeX * aSizeY * aSizeZ]())
	{
		memset(mData, aDefualt, aSizeX * aSizeY * aSizeZ * sizeof(Type));
	};
	
	~Array3D()
	{
		delete[] mData;
	}

	Type& At(const uint32& aX, const uint32& aY, const uint32& aZ)
	{
		return mData[aX + aY * mSizeX + aZ * (mSizeX * mSizeY)];
	};

	/// <summary>
	/// IMPORTANT: Resize will delete all currently stored data and create a new array
	/// </summary>
	void Resize(uint32 aSizeX, uint32 aSizeY, uint32 aSizeZ)
	{
		if (mData)
			delete[] mData;

		mSizeX = aSizeX;
		mSizeY = aSizeY;
		mSizeZ = aSizeZ;
		mData = new Type[mSizeX * mSizeY * mSizeZ];
	}

	Type* GetData(){ return mData; };

	const uint32& GetSizeX() { return mSizeX; }
	const uint32& GetSizeY() { return mSizeY; }
	const uint32& GetSizeZ() { return mSizeZ; }
public:
	Type& operator[](const int32& aIndex)
	{
		return mData[aIndex];
	}

	Type& operator[](const int32& aIndex) const
	{
		return mData[aIndex];
	}

private:
	uint32 mSizeX;
	uint32 mSizeY;
	uint32 mSizeZ;
	Type* mData;
};