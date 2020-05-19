#ifndef TYPHON_FRAMEWORK_HPP
#define TYPHON_FRAMEWORK_HPP

#include <SPeMPE/SPeMPE.hpp>

struct GameContextExtension {

};

class NonstateObject : public spempe::NonstateObject {
public:
    using spempe::NonstateObject::NonstateObject;
};

class StateObject : public spempe::StateObject {
public:
    using spempe::StateObject::StateObject;
};

class SynchronizedObject : public spempe::SynchronizedObject {
public:
    using spempe::SynchronizedObject::SynchronizedObject;
};

#endif // !TYPHON_FRAMEWORK_HPP

