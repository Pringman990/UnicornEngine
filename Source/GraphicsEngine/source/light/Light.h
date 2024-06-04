#pragma once

class Light
{
public:
	Light(const Transform& aTransform, const Color& aColor, float aIntensity);
	virtual ~Light() = default;

	Transform& GetTransform();
	float GetIntensity() const;
	Color GetColor() const;

	void SetColor(Color aNewColor);
	void SetIntensity(float aNewIntensity);

protected:
	Transform mTransform;
	Color mColor;
	float mIntensity;
};