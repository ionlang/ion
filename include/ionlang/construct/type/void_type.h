#pragma once

#include <ionlang/construct/type.h>

namespace ionlang {
    class Pass;

    class VoidType : public Type {
    public:
        VoidType();

        void accept(Pass &pass) override;
    };
}
