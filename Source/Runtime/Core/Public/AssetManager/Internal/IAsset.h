#pragma once

class IAsset
{
public:
	virtual ~IAsset() = default;

	virtual void Free() = 0;
	virtual const String& GetName() const = 0;

private:

};