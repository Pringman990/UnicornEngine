#include "Singleton.h"

namespace {

void __declspec(dllexport) i()
{

}
}

SINGLETON_API std::unordered_map<std::type_index, void*> __sSingletons;