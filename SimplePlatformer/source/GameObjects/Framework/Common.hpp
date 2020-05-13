#ifndef GAME_OBJECT_FRAMEWORK_COMMON_HPP
#define GAME_OBJECT_FRAMEWORK_COMMON_HPP

#include <Hobgoblin/QAO.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_Macros.hpp>
#include <Hobgoblin/Utility/NoCopyNoMove.hpp>

#include <cstdint>
#include <typeinfo>

// All the names in these namespaces start with QAO_ and RN_ anyway...
using namespace hg::qao;
using namespace hg::rn;

#define TYPEID_SELF typeid(decltype(*this))

class GOF_Base;
class GOF_NonstateObject;
class GOF_StateObject;
class GOF_SynchronizedObject;
class GOF_SynchronizedObjectRegistry;

using GOF_SyncId = std::int64_t;
constexpr GOF_SyncId GOF_SYNC_ID_CREATE_MASTER = 0;

#endif // !GAME_OBJECT_FRAMEWORK_COMMON_HPP

