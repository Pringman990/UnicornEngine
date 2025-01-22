#pragma once
#include "RenderCommandList.h"
#include "RenderTarget/RenderTarget.h"
#include "Lights/AmbientLight.h"

struct RenderPassAttachment
{
	RenderPassAttachment() : shouldClear(true) {}

	std::optional<RenderTarget*> rendertarget;

	AmbientLight* ambientlights;

	bool shouldClear;
};

class RenderPass
{
public:
	void AddAttachment(const RenderPassAttachment& aAttachment)
	{
		mAttachments.push_back(aAttachment);
	}

	void AddCommandList(const RenderCommandList& aCommandList)
	{
		mCommandLists.push_back(aCommandList);
	}

	const std::vector<RenderPassAttachment>& GetAttachments() const { return mAttachments; };
	const std::vector<RenderCommandList>& GetCommandLists() const { return mCommandLists; };

private:
	std::vector<RenderPassAttachment> mAttachments;
	std::vector<RenderCommandList> mCommandLists;
};