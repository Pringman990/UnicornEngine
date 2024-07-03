#pragma once

class Scene
{
public:
	Scene();
	Scene(uint32_t aId);
	Scene(uint32_t aId, const std::string& aName);
	~Scene();

	ecs::World& GetWorld();
	void SetName(const std::string& aName);
	uint32_t GetID() const;
private:
	ecs::World mWorld;
	std::string mName;
	uint32_t mId;
}; 