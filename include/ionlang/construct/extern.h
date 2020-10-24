#pragma once

#include <ionlang/construct/pseudo/child_construct.h>
#include "prototype.h"
#include "module.h"

namespace ionlang {
    struct Pass;

    struct Extern : ConstructWithParent<Module> {
        static std::shared_ptr<Extern> make(
            const std::shared_ptr<Prototype>& prototype
        ) noexcept;

        std::shared_ptr<Prototype> prototype;

        explicit Extern(std::shared_ptr<Prototype> prototype);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
