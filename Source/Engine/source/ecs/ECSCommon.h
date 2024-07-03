#pragma once
#include <typeindex>
#include <functional>
#include <unordered_map>

namespace ecs
{
	class World;
#undef max

	using Signature = uint32_t;
	constexpr Signature INVALID_SIGNATURE = std::numeric_limits<Signature>::max();

	using ComponentMask = Signature;
	constexpr ComponentMask INVALID_COMPONENT = std::numeric_limits<ComponentMask>::max();
	
	using ArchetypeMask = Signature;
	constexpr ArchetypeMask INVALID_ARCHETYPE = std::numeric_limits<ArchetypeMask>::max();

	using Entity = uint32_t;
	constexpr Entity INVALID_ENTITY = std::numeric_limits<Entity>::max();

	//Pipeline
	using Pipeline = uint32_t;
	constexpr Pipeline INVALID_PIPELINE = std::numeric_limits<Pipeline>::max();
	const constexpr Pipeline OnLoad = 0;
	const constexpr Pipeline OnPostLoad = 1;
	const constexpr Pipeline OnPreUpdate = 2;
	const constexpr Pipeline OnUpdate = 3;
	const constexpr Pipeline OnPostUpdate = 4;
	const constexpr Pipeline OnRender = 5;
	const constexpr Pipeline PIPELINE_COUNT = 6; //Update when new pipeline is added
	
	using ComponentType = std::type_index;

	template<typename... Components>
	using SystemFunctionT = std::function<void(World&, Entity, Components&...)>;
	using SystemFunction = std::function<void(World&, Entity)>;
	
	struct System final
	{
		Pipeline pipeline = OnUpdate;
		std::string name = "";
		Signature signature = 0;
		SystemFunction function;
	};

	using PipelineSystemMap = std::unordered_map<Pipeline, std::vector<std::shared_ptr<System>>>;
	using NameSystemMap = std::unordered_map<std::string, System>;
}

namespace ecs
{
	namespace internal
	{
		class ComponentRegistry final
		{
		public:
			static ComponentRegistry& GetInstance()
			{
				static ComponentRegistry instance;
				return instance;
			}

			template<typename T>
			ComponentMask TryGetMask()
			{
				auto typeIndex = std::type_index(typeid(T));
				auto it = mMasks.find(typeIndex);
				if (it == mMasks.end())
				{
					ComponentMask newMask = 1 << mNextMask++;
					mMasks[typeIndex] = newMask;
					return newMask;
				}
				return it->second;
			}

			ComponentMask TryGetMask(std::type_index aComponentType)
			{
				auto it = mMasks.find(aComponentType);
				if (it == mMasks.end())
				{
					ComponentMask newMask = 1 << mNextMask++;
					mMasks[aComponentType] = newMask;
					return newMask;
				}
				return it->second;
			}

			template<typename ...Components>
			ecs::Signature CalulateSignature()
			{
				Signature signature = 0;
				((signature |= (1 << TryGetMask<Components>())), ...);
				return signature;
			}

		private:
			ComponentRegistry() : mNextMask(0) {};
			~ComponentRegistry() {};

		private:
			std::unordered_map<std::type_index, ComponentMask> mMasks;
			ComponentMask mNextMask;
		};
	}
}