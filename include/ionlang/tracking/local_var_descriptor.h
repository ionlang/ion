#pragma once

#include <string>
#include <ionlang/construct/type.h>

namespace ionlang {
    struct LocalVariableDescriptor {
        std::string name;

        ionshared::Ptr<Type> type;
    };
}
