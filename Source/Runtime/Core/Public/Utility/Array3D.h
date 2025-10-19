#pragma once
#include <algorithm>

/**
* Helper class to handle 3 dimensional arrays.
* @note 
* Stores a flat C array.
*/
template<typename Type>
class Array3D
{
public:
	Array3D() : mSizeX(0), mSizeY(0), mSizeZ(0), mData(nullptr) {};
	
	Array3D(uint32 SizeX, uint32 SizeY, uint32 SizeZ)
		: mSizeX(SizeX), mSizeY(SizeY), mSizeZ(SizeZ), mData(new Type[SizeX * SizeY * SizeZ]) {};
	
	Array3D(uint32 SizeX, uint32 SizeY, uint32 SizeZ, Type Defualt)
		: mSizeX(SizeX), mSizeY(SizeY), mSizeZ(SizeZ), mData(new Type[SizeX * SizeY * SizeZ]())
	{
		memset(mData, Defualt, SizeX * SizeY * SizeZ * sizeof(Type));
	};
	
	~Array3D()
	{
		delete[] mData;
	}

	Type& At(const uint32& X, const uint32& Y, const uint32& Z)
	{
		return mData[X + Y * mSizeX + Z * (mSizeX * mSizeY)];
	};

	/**
	* Resizes the array in all three axis.
	*
	* @warning 
	* Resize will delete all currently stored data and create a new array.
	*/
	void Resize(uint32 SizeX, uint32 SizeY, uint32 SizeZ)
	{
		if (mData)
			delete[] mData;

		mSizeX = SizeX;
		mSizeY = SizeY;
		mSizeZ = SizeZ;
		mData = new Type[mSizeX * mSizeY * mSizeZ];
	}

	Type* GetData(){ return mData; };

	const uint32& GetSizeX() { return mSizeX; }
	const uint32& GetSizeY() { return mSizeY; }
	const uint32& GetSizeZ() { return mSizeZ; }
public:
	Type& operator[](const int32& Index)
	{
		return mData[Index];
	}

	Type& operator[](const int32& Index) const
	{
		return mData[Index];
	}

private:
	uint32 mSizeX;
	uint32 mSizeY;
	uint32 mSizeZ;
	Type* mData;
};