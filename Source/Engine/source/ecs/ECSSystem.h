#pragma once
#include "ECSCommon.h"

namespace ecs
{
	class World;

	class System final
	{
		template<typename Components>
		using SystemFunction = std::function<void>(World&, Entity, Components&...);

	public:
		System();
		~System();

		void SetPipeline(Pipeline aPipeline);

	private:
		World* mWorld;
		Signature mSignature;

		template<typename Components>
		SystemFunction<Components> mFunction; 
	};
}