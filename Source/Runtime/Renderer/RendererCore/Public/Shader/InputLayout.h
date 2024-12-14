#pragma once

enum class eInputLayoutStandardTypes
{
	eModel = 1,
	eSprite = 2
};

class InputLayout
{
public:
	InputLayout(eInputLayoutStandardTypes aType) : mType(aType) {};
	virtual ~InputLayout() {};

	virtual void Bind() = 0;

	const eInputLayoutStandardTypes GetType() const { return mType; };

private:
	eInputLayoutStandardTypes mType;
};