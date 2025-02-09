#pragma once

class Texture
{
public:
	Texture() = default;
	virtual ~Texture() = default;

	virtual void BindPS(uint32 aSlot) const = 0;
	virtual void BindCS(uint32 aSlot) const = 0;
	virtual void UnbindCS(uint32 aSlot) const = 0;
private:
};