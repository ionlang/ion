#pragma once

#include <ionlang/construct/type.h>

namespace ionlang {
    struct Pass;

    struct VoidType : Type {
        VoidType();

        void accept(Pass& pass) override;
    };
}
