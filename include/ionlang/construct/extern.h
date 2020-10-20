#pragma once

#include <ionlang/construct/pseudo/child_construct.h>
#include "prototype.h"
#include "module.h"

namespace ionlang {
    class Pass;

    struct Extern : ConstructWithParent<Module> {
        std::shared_ptr<Prototype> prototype;

        Extern(
            std::shared_ptr<Module> parent,
            std::shared_ptr<Prototype> prototype
        );

        void accept(Pass &visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
