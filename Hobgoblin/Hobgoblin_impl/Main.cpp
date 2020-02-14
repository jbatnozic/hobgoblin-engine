
#include <Hobgoblin_include/Macros.hpp>
#include <Hobgoblin_include/QAO.hpp>

#include <cassert>
#include <iostream>
#include <memory>

using namespace jbatnozic::hobgoblin::qao;

class Derived : public QAO_Base {
public:
    Derived(int number) 
        : QAO_Base{0, 0, "Derived"}
        , QAO_PROPINIT(number1, number)
        , QAO_PROPINIT(number2)
    {
    }

    ~Derived() {
        std::cout << "Derived " << _number1 << " destroyed.\n";
    }

    void printSomething() const {
        std::cout << "Something\n";
    }

    void eventUpdate(QAO_Runtime& rt) override {
        std::cout << "Derived " << _number1 << '\n';
    }

private:
    QAO_PROPDECL(int, number1);
    QAO_PROPDECL(int, number2);
    //int _number;
};

constexpr int SIZE = sizeof(Derived);

int main() {

    /*
    1)
    QAO_Runtime& runtime;
    auto id = QAO_Create<Class>(runtime, ...);
    ...
    QAO_Delete(id);

    2)
    QAO_Runtime& runtime;
    auto id = QAO_Create<Class>(runtime, ...);
    ...
    auto uPtr = runtime.releaseObject(id);
    ...
    uPtr.reset();

    3)
    QAO_Runtime& runtime;
    Class obj{...};
    runtime.addObjectNoOwn(obj);
    ...
    runtime.releaseObject(obj); <- Also implicit in obj's destructor
    */

    {
        QAO_Runtime rt;
        Derived stack_derived{-1};
        rt.addObjectNoOwn(stack_derived);
        auto obj_1 = QAO_Create<Derived>(rt, 1);
        auto obj_2 = QAO_Create<Derived>(rt, 2);
        auto obj_3 = QAO_Create<Derived>(rt, 3);
        
        bool done;
        rt.startStep();
        rt.advanceStep(done);
        assert(done);

        obj_3->setExecutionPriority(5);

        auto uPtr = rt.releaseObject(obj_2->getId());

        rt.startStep();
        rt.advanceStep(done);
        assert(done);

        uPtr.reset();
    }

    std::cout << "Happy end\n";
    std::cin.get();
}