#pragma once

#include <ionlang/construct/type.h>

namespace ionlang {
    class Pass;

    struct VoidType : Type {
        VoidType();

        void accept(Pass &pass) override;
    };
}
