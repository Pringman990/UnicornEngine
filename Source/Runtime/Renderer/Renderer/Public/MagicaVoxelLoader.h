#pragma once


class MagicaVoxelLoader
{
	struct Chunk
	{
		char id[4];
	};

	struct FileStructure
	{
		char fileFormat[4];
		int32 versionNumber;


	};
public:
	MagicaVoxelLoader();
	~MagicaVoxelLoader();

private:

};