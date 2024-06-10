#pragma once

namespace ecs
{
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
	const constexpr Pipeline PIPELINE_COUNT = 5; //Update when new pipeline is added
	
	using ComponentType = std::type_index;
	
	struct Archetype final
	{
		/// <summary>
		/// Signature of the archetype
		/// tells us what components that exists in the current archetype
		/// </summary>
		ComponentMask signature;

		/// <summary>
		/// Type index to a pointer to a std::vector
		/// of all components of that type
		/// </summary>
		std::unordered_map<ComponentType, void*> componentArrays;

		/// <summary>
		/// To get a specific entity's component
		/// we map a Entity to a map of component types and a pointer
		/// to the start of that component in memory
		/// </summary>
		std::unordered_map<Entity, std::unordered_map<ComponentType, void*>> entityComponents;

		void AddEntity(Entity /*entity*/)
		{

		}

		template<typename T>
		T GetComponent(Entity aEntity)
		{
			auto componentType = std::type_index(typeid(T));
			return static_cast<T>(entityComponents[aEntity][componentType]);
		}
	};
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

		private:
			ComponentRegistry() : mNextMask(0) {};
			~ComponentRegistry() {};

		private:
			std::unordered_map<std::type_index, ComponentMask> mMasks;
			ComponentMask mNextMask;
		};
	}
}