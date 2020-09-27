#pragma once

#include <ionlang/construct/pseudo/child_construct.h>
#include "prototype.h"
#include "module.h"

namespace ionlang {
    class Pass;

    struct Extern : ConstructWithParent<Module> {
        ionshared::Ptr<Prototype> prototype;

        Extern(
            ionshared::Ptr<Module> parent,
            ionshared::Ptr<Prototype> prototype
        );

        void accept(Pass &visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
