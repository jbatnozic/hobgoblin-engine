#ifndef OBJECT_FRAMEWORK_HPP
#define OBJECT_FRAMEWORK_HPP

#include <Hobgoblin/QAO.hpp>

namespace hg = jbatnozic::hobgoblin;
using namespace hg::qao;

struct GlobalProgramState;

class GameObject : public QAO_Base {
public:
    using QAO_Base::QAO_Base;
    GlobalProgramState& global() const;
};

#endif // !OBJECT_FRAMEWORK_HPP
