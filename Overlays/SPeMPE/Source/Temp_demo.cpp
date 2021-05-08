
#include <Hobgoblin/Utility/Autopack.hpp>
#include <SPeMPE/SPeMPE.hpp>

#define SPEMPEIMPL_MACRO_CONCAT_WITHARG(_x_, _y_, _arg_) _x_##_y_(_arg_)
#define SPEMPEIMPL_MACRO_EXPAND(_x_) _x_
#define SPEMPEIMPL_MACRO_EXPAND_VA(...) __VA_ARGS__

#define SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_EMPTY(_class_name_)
#define SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_CREATE(_class_name_)  create##_class_name_
#define SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_UPDATE(_class_name_)  update##_class_name_
#define SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_DESTROY(_class_name_) destroy##_class_name_

#define SPEMPEIMPL_GENERATE_MULTIPLE_DEFAULT_SYNC_HANDLERS(_class_name_, _tag_1_, _tag_2_, _tag_3_, ...) \
    SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_##_tag_1_ (_class_name_) \
    SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_##_tag_2_ (_class_name_) \
    SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_##_tag_3_ (_class_name_)

#define SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLERS(_class_name_, ...) \
    SPEMPEIMPL_MACRO_EXPAND( \
        SPEMPEIMPL_GENERATE_MULTIPLE_DEFAULT_SYNC_HANDLERS(_class_name_, __VA_ARGS__, EMPTY, EMPTY) \
    )

#define SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(_class_name_, _for_events_) \
    SPEMPEIMPL_MACRO_EXPAND( \
        SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLERS(_class_name_, SPEMPEIMPL_MACRO_EXPAND_VA _for_events_) \
    )

namespace jbatnozic {
namespace spempe {

template <class T>
void DefaultSyncCreateImpl(hg::RN_NodeInterface& aNode,
                           const std::vector<hg::PZInteger>& aRecepients,
                           T* self) {
    // ...
}

}
}

namespace hg  = ::jbatnozic::hobgoblin;
namespace spe = ::jbatnozic::spempe;

//////////////////////////////////////////////////////
// HEADER

struct MyVisibleState {
    double x;
    double y;

    HG_ENABLE_AUTOPACK(MyVisibleState, x, y);
};

class MyGameObject : public spe::SynchronizedObject<MyVisibleState> {
public:
protected:
    void eventUpdate(spe::IfMaster) override {
        // ...
    }

    void eventDraw1() override {
        // ...
    }

private:
    void _syncCreateImpl( hg::RN_NodeInterface& aNode,
                          const std::vector<hg::PZInteger>& aRecepients) const override;

    void _syncUpdateImpl( hg::RN_NodeInterface& aNode,
                          const std::vector<hg::PZInteger>& aRecepients) const override;

    void _syncDestroyImpl(hg::RN_NodeInterface& aNode,
                          const std::vector<hg::PZInteger>& aRecepients) const override;
};

//////////////////////////////////////////////////////
// SOURCE

int SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(MyGameObject, (CREATE)) = 0;

void MyGameObject::_syncCreateImpl( hg::RN_NodeInterface& aNode,
                                    const std::vector<hg::PZInteger>& aRecepients) const {
    spe::DefaultSyncCreateImpl(aNode, aRecepients, this);
}

void MyGameObject::_syncUpdateImpl( hg::RN_NodeInterface& aNode,
                                    const std::vector<hg::PZInteger>& aRecepients) const {
    // ...
}

void MyGameObject::_syncDestroyImpl(hg::RN_NodeInterface& aNode,
                                    const std::vector<hg::PZInteger>& aRecepients) const {
    // ...
}

#define STRINGIZE_(x) #x
#define STRINGIZE(x)  STRINGIZE_(x)

#pragma message("MSG: " STRINGIZE(SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(MyGameObject, (CREATE, DESTROY, UPDATE))))