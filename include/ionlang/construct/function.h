#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/tracking/local_var_descriptor.h>
#include "construct.h"
#include "prototype.h"
#include "module.h"

namespace ionlang {
    class Pass;

    struct Function : ConstructWithParent<Module> {
        std::shared_ptr<Prototype> prototype;

        std::shared_ptr<Block> body;

        ionshared::PtrSymbolTable<LocalVariableDescriptor> localVariables;

        Function(
            std::shared_ptr<Module> parent,
            std::shared_ptr<Prototype> prototype,
            std::shared_ptr<Block> body
        );

        void accept(Pass &visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
