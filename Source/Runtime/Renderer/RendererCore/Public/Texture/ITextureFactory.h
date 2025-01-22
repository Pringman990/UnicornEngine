#pragma once
#include "Texture/Texture.h"
#include <string>

class ITextureFactory
{
public:
	ITextureFactory() {};
	virtual ~ITextureFactory() {};

	virtual Texture* LoadTextureFromFile(const std::string& aPath) = 0;

private:

};