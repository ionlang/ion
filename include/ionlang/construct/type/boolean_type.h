#pragma once

#include <ionlang/construct/type.h>

namespace ionlang {
    class Pass;

    class BooleanType : public Type {
    public:
        explicit BooleanType();

        void accept(Pass &pass) override;
    };
}
