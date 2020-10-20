#pragma once

#include <string>
#include <ionlang/construct/type.h>

namespace ionlang {
    struct LocalVariableDescriptor {
        std::string name;

        std::shared_ptr<Type> type;
    };
}
