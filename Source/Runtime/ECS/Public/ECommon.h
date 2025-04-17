#pragma once
#include <Core.h>

class EWorld;
#undef max

using ESignature = uint32_t;
constexpr ESignature INVALID_SIGNATURE = std::numeric_limits<ESignature>::max();

using EComponentMask = ESignature;
constexpr EComponentMask INVALID_COMPONENT = std::numeric_limits<EComponentMask>::max();

using EArchetypeMask = ESignature;
constexpr EArchetypeMask INVALID_ARCHETYPE = std::numeric_limits<EArchetypeMask>::max();

using EEntity = uint32_t;
constexpr EEntity INVALID_ENTITY = std::numeric_limits<EEntity>::max();

using EPipeline = uint32_t;
constexpr EPipeline INVALID_PIPELINE = std::numeric_limits<EPipeline>::max();
const constexpr EPipeline OnLoad = 0;
const constexpr EPipeline OnPostLoad = 1;
const constexpr EPipeline OnPreUpdate = 2;
const constexpr EPipeline OnUpdate = 3;
const constexpr EPipeline OnPostUpdate = 4;
const constexpr EPipeline OnRender = 5;
const constexpr EPipeline PIPELINE_COUNT = 6;

using EComponentType = std::type_index;

template<typename... EComponents>
using ESystemFunctionT = std::function<void(EWorld&, EEntity, EComponents&...)>;
using ESystemFunction = std::function<void(EWorld&, EEntity)>;

struct ESystem final
{
	EPipeline pipeline = OnUpdate;
	std::string name = "";
	ESignature signature = 0;
	ESystemFunction function;
};

using EPipelineSystemMap = std::unordered_map<EPipeline, std::vector<std::shared_ptr<ESystem>>>;
using ENameSystemMap = std::unordered_map<std::string, ESystem>;



namespace internal
{
	class EComponentRegistry final
	{
	public:
		static EComponentRegistry& GetInstance()
		{
			static EComponentRegistry instance;
			return instance;
		}

		template<typename T>
		EComponentMask TryGetMask()
		{
			auto typeIndex = std::type_index(typeid(T));
			auto it = mMasks.find(typeIndex);
			if (it == mMasks.end())
			{
				EComponentMask newMask = 1 << mNextMask++;
				mMasks[typeIndex] = newMask;
				return newMask;
			}
			return it->second;
		}

		EComponentMask TryGetMask(std::type_index aComponentType)
		{
			auto it = mMasks.find(aComponentType);
			if (it == mMasks.end())
			{
				EComponentMask newMask = 1 << mNextMask++;
				mMasks[aComponentType] = newMask;
				return newMask;
			}
			return it->second;
		}

		template<typename ...EComponents>
		ESignature CalulateSignature()
		{
			ESignature signature = 0;
			((signature |= (1 << TryGetMask<EComponents>())), ...);
			return signature;
		}

	private:
		EComponentRegistry() : mNextMask(0) {};
		~EComponentRegistry() {};

	private:
		std::unordered_map<std::type_index, EComponentMask> mMasks;
		EComponentMask mNextMask;
	};
}
