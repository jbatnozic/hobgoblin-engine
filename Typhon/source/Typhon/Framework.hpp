#ifndef TYPHON_FRAMEWORK_HPP
#define TYPHON_FRAMEWORK_HPP

#include <SPeMPE/SPeMPE.hpp>

#include <Typhon/Framework/Collisions.hpp>
#include <Typhon/Framework/Execution_priorities.hpp>
#include <Typhon/Framework/Extensions.hpp>

#define TYPEID_SELF SPEMPE_TYPEID_SELF

// Everything in these namespaces is prefixed with QAO_ and RN_ anyway...
using namespace hg::qao;
using namespace hg::rn;

using spempe::GameContext;
using spempe::KbKey;
using spempe::NetworkingManager;
using spempe::SynchronizedObjectRegistry;
using spempe::SyncId;
using spempe::SYNC_ID_NEW;
using spempe::WindowManager;

class NonstateObject : public spempe::NonstateObject {
public:
    using spempe::NonstateObject::NonstateObject;
    using spempe::NonstateObject::ctx;
    
    PASTE_CONTEXT_TAG_DEFINITIONS;
    PASTE_CTX_METHOD_OVERLOADS;
};

class StateObject : public spempe::StateObject {
public:
    using spempe::StateObject::StateObject;
    using spempe::StateObject::ctx;
    
    PASTE_CONTEXT_TAG_DEFINITIONS;
    PASTE_CTX_METHOD_OVERLOADS;
};

class SynchronizedObject : public spempe::SynchronizedObject {
public:
    using spempe::SynchronizedObject::SynchronizedObject;
    using spempe::SynchronizedObject::ctx;
    
    PASTE_CONTEXT_TAG_DEFINITIONS;
    PASTE_CTX_METHOD_OVERLOADS;
};

#undef PASTE_CONTEXT_TAG_DEFINITIONS
#undef PASTE_CTX_METHOD_OVERLOADS

#endif // !TYPHON_FRAMEWORK_HPP

