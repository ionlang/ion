#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/tracking/local_var_descriptor.h>
#include "construct.h"
#include "prototype.h"
#include "module.h"
#include "block.h"

namespace ionlang {
    struct Pass;

    struct Function : ConstructWithParent<Module, Construct, ConstructKind> {
        static std::shared_ptr<Function> make(
            const std::shared_ptr<Prototype>& prototype,
            const std::shared_ptr<Block>& body
        ) noexcept;

        std::shared_ptr<Prototype> prototype;

        std::shared_ptr<Block> body;

        Function(
            std::shared_ptr<Prototype> prototype,
            std::shared_ptr<Block> body
        );

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
