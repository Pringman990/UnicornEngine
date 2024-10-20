#pragma once

class IRenderTargetBackend
{
public:
	IRenderTargetBackend() : mSize(Vector2::Zero), mEnabledDepthTesting(true) {};
	virtual ~IRenderTargetBackend() {};

	virtual void Resize(Vector2 aSize) = 0;

	/// <summary>
	/// INTERNAL USE ONLY. 
	/// Use Resize to set the size
	/// </summary>
	void SetSize(Vector2 aSize) { mSize = aSize; };
	const Vector2& GetSize() { return mSize; };
	
	bool IsDepthTesting() { return mEnabledDepthTesting; }
	void SetEnabledDepthTesting(bool EnabledDepthTesting) { mEnabledDepthTesting = EnabledDepthTesting; };

protected:
	Vector2 mSize;
	bool mEnabledDepthTesting;
};