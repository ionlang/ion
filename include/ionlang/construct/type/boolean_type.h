#pragma once

#include <ionlang/construct/type.h>

namespace ionlang {
    class Pass;

    class BooleanType : public Type {
    public:
        explicit BooleanType(bool isPointer = false);

        void accept(Pass &pass) override;
    };
}
