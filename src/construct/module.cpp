#include <ionlang/construct/module.h>

namespace ionlang {
    Module::Module(std::string id) : Construct(ConstructKind::Module), ionshared::Named(id) {
        //
    }
}
