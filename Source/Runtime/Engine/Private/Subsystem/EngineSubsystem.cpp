#include "Subsystem/EngineSubsystem.h"
#include <iostream>

namespace 
{
	static std::mutex sMutex;

	ENGINE_API UnorderedMap<std::type_index, __Subsystem::Singleton>& GetSingletons()
	{
		static UnorderedMap<std::type_index, __Subsystem::Singleton> singletons;
		return singletons;
	}

	ENGINE_API UnorderedMap<String, __Subsystem::Singleton*>& GetSingletonsName()
	{
		static UnorderedMap<String, __Subsystem::Singleton*> singletons;
		return singletons;
	}

	bool DFSVisit(
		const String& node,
		const UnorderedMap<String, Vector<String>>& graph,
		UnorderedSet<String>& visited,
		UnorderedSet<String>& stack,
		Vector<String>& order,
		Vector<String>& cycleNodes
	)
	{
		if (stack.count(node)) 
		{
			// cycle detected, add node to cycle list
			cycleNodes.push_back(node);
			return false;
		}

		if (visited.count(node))
			return true;

		visited.insert(node);
		stack.insert(node);

		auto it = graph.find(node);
		if (it != graph.end())
		{
			for (const auto& dep : it->second) 
			{
				if (!DFSVisit(dep, graph, visited, stack, order, cycleNodes)) 
				{
					cycleNodes.push_back(node); // add current node to cycle
					return false;
				}
			}
		}

		stack.erase(node);
		order.push_back(node); // add after dependencies
		return true;
	}

	Vector<String> TopologicSortWithCycle(UnorderedMap<String, Vector<String>>& Graph)
	{
		UnorderedSet<String> visited;
		UnorderedSet<String> stack;
		Vector<String> order;
		Vector<String> cycleNodes;

		for (const auto& [node,_] : Graph)
		{
			if (!visited.count(node))
			{
				if (!DFSVisit(node, Graph, visited, stack, order, cycleNodes))
				{
					LOG_ERROR("Cyclic dependency detected: ");
					for (auto it = cycleNodes.rbegin(); it != cycleNodes.rend(); ++it)
					{
						LOG_ERROR("{}", *it);
					}
					THROW("Cyclic subsystem dependency!");
				}
			}
		}

		return order;
	}

}

namespace __Subsystem
{
	ENGINE_API void CreateAllSystems()
	{
		UnorderedMap<String, Vector<String>> graph;
		for (auto& [type, sing] : GetSingletons())
		{
			auto& deps = graph[String(sing.desc->name)];
			deps.assign(sing.desc->dependencies.begin(), sing.desc->dependencies.end());
		}

		Vector<String> order = TopologicSortWithCycle(graph);

		for (const String& sys : order)
		{
			auto& names = GetSingletonsName();
			__Subsystem::Singleton* sing = names[sys];
			if (sing->desc->constructor)
			{
				sing->instance = malloc(sing->desc->size);
				sing->desc->constructor(sing->instance);
				continue;
			}
			THROW("Trying to create subsystem with null constructor: {}", sing->desc->name);
		}

		LOG_INFO("All engine subsystems have been created");
	}

	ENGINE_API void Register(std::type_index Type, const subsystem::SubsystemDescriptor* Desc)
	{
		std::scoped_lock lock(sMutex);
		ASSERT(GetSingletons().contains(Type) == false, "Subsystem has already been registered");

		__Subsystem::Singleton sing;
		sing.desc = Desc;
		sing.instance = nullptr;

		GetSingletons()[Type] = std::move(sing);
		GetSingletonsName()[String(Desc->name)] = &GetSingletons()[Type];
	}

	ENGINE_API void* Get(const std::type_index& Type)
	{
		std::scoped_lock lock(sMutex);
		auto it = GetSingletons().find(Type);
		return it != GetSingletons().end() ? it->second.instance : nullptr;
	}

	ENGINE_API UnorderedMap<std::type_index, __Subsystem::Singleton>& GetAll()
	{
		return GetSingletons();
	}

	ENGINE_API void Unregister(const std::type_index& Type)
	{
		std::scoped_lock lock(sMutex);
		auto it = GetSingletons().find(Type);
		if (it != GetSingletons().end())
		{
			it->second.desc->destructor(it->second.instance);
		}
	}

	ENGINE_API void EnsureAllShutdown()
	{
		ASSERT(GetSingletons().size() <= 0, "");
	}
}
